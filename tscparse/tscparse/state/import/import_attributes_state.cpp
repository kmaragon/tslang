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

#include "import_attributes_state.hpp"
#include <stdexcept>
#include <tsclex/tokens/as_token.hpp>
#include <tsclex/tokens/assert_token.hpp>
#include <tsclex/tokens/close_brace_token.hpp>
#include <tsclex/tokens/colon_token.hpp>
#include <tsclex/tokens/comma_token.hpp>
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/from_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/open_brace_token.hpp>
#include <tsclex/tokens/require_token.hpp>
#include <tsclex/tokens/type_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"

using namespace tscc::parse::state;

import_attributes_state::import_attributes_state(import_node_builder* builder)
	: builder_(builder) {}

class import_attributes_state::expect_open_brace_visitor
	: public basic_state_visitor {
public:
	expect_open_brace_visitor(import_attributes_state* s,
							  const lex::source_location& loc,
							  const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::open_brace_token&) const {
		s_->phase_ = phase::expect_key_or_close;
		return state_result::stay();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "'{'", token_->to_string());
	}

private:
	import_attributes_state* s_;
	const lex::token& token_;
};

class import_attributes_state::expect_key_or_close_visitor
	: public basic_state_visitor {
public:
	expect_key_or_close_visitor(import_attributes_state* s,
								const lex::source_location& loc,
								const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::close_brace_token&) const {
		return state_result::complete(nullptr);
	}

	state_result operator()(const lex::tokens::identifier_token&) const {
		return handle_key();
	}
	state_result operator()(const lex::tokens::type_token&) const {
		return handle_key();
	}
	state_result operator()(const lex::tokens::from_token&) const {
		return handle_key();
	}
	state_result operator()(const lex::tokens::as_token&) const {
		return handle_key();
	}
	state_result operator()(const lex::tokens::assert_token&) const {
		return handle_key();
	}
	state_result operator()(const lex::tokens::require_token&) const {
		return handle_key();
	}
	state_result operator()(const lex::tokens::constant_value_token&) const {
		return handle_key();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "attribute key or '}'",
							 token_->to_string());
	}

private:
	state_result handle_key() const {
		s_->pending_key_ = token_;
		s_->phase_ = phase::after_key;
		return state_result::stay();
	}

	import_attributes_state* s_;
	const lex::token& token_;
};

class import_attributes_state::after_key_visitor : public basic_state_visitor {
public:
	after_key_visitor(import_attributes_state* s,
					  const lex::source_location& loc,
					  const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::colon_token&) const {
		s_->phase_ = phase::after_colon;
		return state_result::stay();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "':'", token_->to_string());
	}

private:
	import_attributes_state* s_;
	const lex::token& token_;
};

class import_attributes_state::after_colon_visitor
	: public basic_state_visitor {
public:
	after_colon_visitor(import_attributes_state* s,
						const lex::source_location& loc,
						const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::constant_value_token&) const {
		s_->builder_->add_attribute(std::move(*s_->pending_key_), token_);
		s_->pending_key_.reset();
		s_->phase_ = phase::after_value;
		return state_result::stay();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "string literal", token_->to_string());
	}

private:
	import_attributes_state* s_;
	const lex::token& token_;
};

class import_attributes_state::after_value_visitor
	: public basic_state_visitor {
public:
	after_value_visitor(import_attributes_state* s,
						const lex::source_location& loc,
						const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::comma_token&) const {
		s_->phase_ = phase::expect_key_or_close;
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
	import_attributes_state* s_;
	const lex::token& token_;
};

state_result import_attributes_state::process(parser& /*p*/,
											  const lex::token& token) {
	switch (phase_) {
		case phase::expect_open_brace:
			return token.visit(
				expect_open_brace_visitor{this, token.location(), token});
		case phase::expect_key_or_close:
			return token.visit(
				expect_key_or_close_visitor{this, token.location(), token});
		case phase::after_key:
			return token.visit(
				after_key_visitor{this, token.location(), token});
		case phase::after_colon:
			return token.visit(
				after_colon_visitor{this, token.location(), token});
		case phase::after_value:
			return token.visit(
				after_value_visitor{this, token.location(), token});
	}

	throw expected_token(token.location(), "import attribute",
						 token->to_string());
}

accept_result import_attributes_state::accept_child(
	std::unique_ptr<ast::ast_node>) {
	throw std::logic_error(
		"import_attributes_state does not push child states");
}
