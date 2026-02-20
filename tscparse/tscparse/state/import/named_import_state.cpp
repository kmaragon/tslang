/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2026. Keef Aragon
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "named_import_state.hpp"
#include <stdexcept>
#include <tsclex/tokens/as_token.hpp>
#include <tsclex/tokens/assert_token.hpp>
#include <tsclex/tokens/close_brace_token.hpp>
#include <tsclex/tokens/comma_token.hpp>
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/default_token.hpp>
#include <tsclex/tokens/from_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/require_token.hpp>
#include <tsclex/tokens/type_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "../token_helpers.hpp"

using namespace tscc::parse::state;
using detail::can_be_identifier;
using detail::can_be_specifier_name;

named_import_state::named_import_state(import_node_builder* builder)
	: builder_(builder) {}

class named_import_state::expect_specifier_or_close_visitor
	: public basic_state_visitor {
public:
	expect_specifier_or_close_visitor(named_import_state* s,
									  const lex::source_location& loc,
									  const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::close_brace_token&) const {
		return state_result::complete(nullptr);
	}

	state_result operator()(const lex::tokens::type_token&) const {
		s_->pending_type_ = token_;
		s_->phase_ = phase::after_maybe_type;
		return state_result::stay();
	}

	state_result operator()(const lex::tokens::identifier_token&) const {
		return handle_specifier_name();
	}
	state_result operator()(const lex::tokens::from_token&) const {
		return handle_specifier_name();
	}
	state_result operator()(const lex::tokens::as_token&) const {
		return handle_specifier_name();
	}
	state_result operator()(const lex::tokens::assert_token&) const {
		return handle_specifier_name();
	}
	state_result operator()(const lex::tokens::require_token&) const {
		return handle_specifier_name();
	}
	state_result operator()(const lex::tokens::constant_value_token&) const {
		return handle_specifier_name();
	}
	state_result operator()(const lex::tokens::default_token&) const {
		return handle_specifier_name();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "import specifier or '}'",
							 token_->to_string());
	}

private:
	state_result handle_specifier_name() const {
		s_->pending_name_ = token_;
		s_->phase_ = phase::after_name;
		return state_result::stay();
	}

	named_import_state* s_;
	const lex::token& token_;
};

class named_import_state::after_maybe_type_visitor
	: public basic_state_visitor {
public:
	after_maybe_type_visitor(named_import_state* s,
							 const lex::source_location& loc,
							 const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::as_token&) const {
		// "type" is the specifier name: import { type as X }
		s_->pending_name_ = std::move(s_->pending_type_);
		s_->pending_type_.reset();
		s_->phase_ = phase::after_as;
		return state_result::stay();
	}

	state_result operator()(const lex::tokens::comma_token&) const {
		// "type" is a specifier named "type": import { type, ... }
		s_->pending_name_ = std::move(s_->pending_type_);
		s_->pending_type_.reset();
		s_->flush_specifier();
		s_->phase_ = phase::expect_specifier_or_close;
		return state_result::stay();
	}

	state_result operator()(const lex::tokens::close_brace_token&) const {
		// "type" is a specifier named "type": import { type }
		s_->pending_name_ = std::move(s_->pending_type_);
		s_->pending_type_.reset();
		s_->flush_specifier();
		return state_result::complete(nullptr);
	}

	state_result operator()(const lex::tokens::identifier_token&) const {
		return handle_name_after_type();
	}
	state_result operator()(const lex::tokens::from_token&) const {
		return handle_name_after_type();
	}
	state_result operator()(const lex::tokens::assert_token&) const {
		return handle_name_after_type();
	}
	state_result operator()(const lex::tokens::require_token&) const {
		return handle_name_after_type();
	}
	state_result operator()(const lex::tokens::constant_value_token&) const {
		return handle_name_after_type();
	}
	state_result operator()(const lex::tokens::default_token&) const {
		return handle_name_after_type();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location,
							 "import specifier name, 'as', ',', or '}'",
							 token_->to_string());
	}

private:
	// "type" is the type modifier, this token is the specifier name
	state_result handle_name_after_type() const {
		s_->pending_name_ = token_;
		s_->phase_ = phase::after_name;
		return state_result::stay();
	}

	named_import_state* s_;
	const lex::token& token_;
};

class named_import_state::after_name_visitor : public basic_state_visitor {
public:
	after_name_visitor(named_import_state* s,
					   const lex::source_location& loc,
					   const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::as_token&) const {
		s_->phase_ = phase::after_as;
		return state_result::stay();
	}

	state_result operator()(const lex::tokens::comma_token&) const {
		s_->flush_specifier();
		s_->phase_ = phase::expect_specifier_or_close;
		return state_result::stay();
	}

	state_result operator()(const lex::tokens::close_brace_token&) const {
		s_->flush_specifier();
		return state_result::complete(nullptr);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "'as', ',', or '}'",
							 token_->to_string());
	}

private:
	named_import_state* s_;
	const lex::token& token_;
};

class named_import_state::after_as_visitor : public basic_state_visitor {
public:
	after_as_visitor(named_import_state* s,
					 const lex::source_location& loc,
					 const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::identifier_token&) const {
		return handle_alias();
	}
	state_result operator()(const lex::tokens::type_token&) const {
		return handle_alias();
	}
	state_result operator()(const lex::tokens::from_token&) const {
		return handle_alias();
	}
	state_result operator()(const lex::tokens::as_token&) const {
		return handle_alias();
	}
	state_result operator()(const lex::tokens::assert_token&) const {
		return handle_alias();
	}
	state_result operator()(const lex::tokens::require_token&) const {
		return handle_alias();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "identifier", token_->to_string());
	}

private:
	state_result handle_alias() const {
		s_->flush_specifier_with_alias(token_);
		s_->phase_ = phase::after_alias;
		return state_result::stay();
	}

	named_import_state* s_;
	const lex::token& token_;
};

class named_import_state::after_alias_visitor : public basic_state_visitor {
public:
	after_alias_visitor(named_import_state* s,
						const lex::source_location& loc,
						const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::comma_token&) const {
		s_->phase_ = phase::expect_specifier_or_close;
		return state_result::stay();
	}

	state_result operator()(const lex::tokens::close_brace_token&) const {
		return state_result::complete(nullptr);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "',' or '}'", token_->to_string());
	}

private:
	named_import_state* s_;
	const lex::token& token_;
};

state_result named_import_state::process(parser& /*p*/,
										 const lex::token& token) {
	switch (phase_) {
		case phase::expect_specifier_or_close:
			return token.visit(expect_specifier_or_close_visitor{
				this, token.location(), token});
		case phase::after_maybe_type:
			return token.visit(
				after_maybe_type_visitor{this, token.location(), token});
		case phase::after_name:
			return token.visit(
				after_name_visitor{this, token.location(), token});
		case phase::after_as:
			return token.visit(after_as_visitor{this, token.location(), token});
		case phase::after_alias:
			return token.visit(
				after_alias_visitor{this, token.location(), token});
	}

	throw expected_token(token.location(), "import specifier",
						 token->to_string());
}

accept_result named_import_state::accept_child(std::unique_ptr<ast::ast_node>) {
	throw std::logic_error("named_import_state does not push child states");
}

void named_import_state::flush_specifier() {
	ast::import_specifier spec;
	spec.type_keyword = std::move(pending_type_);
	spec.name = std::move(*pending_name_);
	builder_->add_named_specifier(std::move(spec));
	pending_type_.reset();
	pending_name_.reset();
}

void named_import_state::flush_specifier_with_alias(lex::token alias) {
	ast::import_specifier spec;
	spec.type_keyword = std::move(pending_type_);
	spec.name = std::move(*pending_name_);
	spec.alias = std::move(alias);
	builder_->add_named_specifier(std::move(spec));
	pending_type_.reset();
	pending_name_.reset();
}
