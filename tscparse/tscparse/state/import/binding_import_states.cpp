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

#include "binding_import_states.hpp"
#include <stdexcept>
#include <tsclex/tokens/asterisk_token.hpp>
#include <tsclex/tokens/comma_token.hpp>
#include <tsclex/tokens/eq_token.hpp>
#include <tsclex/tokens/from_token.hpp>
#include <tsclex/tokens/open_brace_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "equals_import_states.hpp"
#include "from_clause_states.hpp"
#include "named_import_state.hpp"
#include "namespace_import_states.hpp"

using namespace tscc::parse::state;

after_default_state::after_default_state(import_node_builder* builder,
										 lex::token binding_tok,
										 bool equals_only)
	: builder_(builder),
	  pending_binding_(std::move(binding_tok)),
	  equals_only_(equals_only) {}

class after_default_state::visitor : public basic_state_visitor {
public:
	visitor(after_default_state* s,
			const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::from_token&) const {
		s_->builder_->set_default_binding(std::move(s_->pending_binding_));
		return state_result::push<after_from_state>(s_->builder_);
	}

	state_result operator()(const lex::tokens::comma_token&) const {
		s_->builder_->set_default_binding(std::move(s_->pending_binding_));
		return state_result::push<after_comma_state>(s_->builder_);
	}

	state_result operator()(const lex::tokens::eq_token&) const {
		s_->builder_->set_equals_name(std::move(s_->pending_binding_));
		return state_result::push<after_equals_state>(s_->builder_);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "'from', ',', or '='",
							 token_->to_string());
	}

	using basic_state_visitor::operator();

private:
	after_default_state* s_;
	const lex::token& token_;
};

/// Restricted visitor for namespace scope: only accepts `=`.
class after_default_state::equals_only_visitor : public basic_state_visitor {
public:
	equals_only_visitor(after_default_state* s,
						const lex::source_location& loc,
						const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::eq_token&) const {
		s_->builder_->set_equals_name(std::move(s_->pending_binding_));
		return state_result::push<after_equals_state>(s_->builder_, true);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "'='", token_->to_string());
	}

	using basic_state_visitor::operator();

private:
	after_default_state* s_;
	const lex::token& token_;
};

state_result after_default_state::process(parser& /*p*/,
										  const lex::token& token) {
	if (equals_only_)
		return token.visit(equals_only_visitor{this, token.location(), token});
	return token.visit(visitor{this, token.location(), token});
}

accept_result after_default_state::accept_child(
	std::unique_ptr<ast::ast_node>) {
	return accept_result::complete(nullptr);
}

after_comma_state::after_comma_state(import_node_builder* builder)
	: builder_(builder) {}

class after_comma_state::visitor : public basic_state_visitor {
public:
	visitor(after_comma_state* s,
			const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::asterisk_token&) const {
		return state_result::push<expect_as_state>(s_->builder_);
	}

	state_result operator()(const lex::tokens::open_brace_token&) const {
		s_->builder_->init_named_imports();
		s_->mode_ = mode::awaiting_sub;
		return state_result::push<named_import_state>(s_->builder_);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "'*' or '{'", token_->to_string());
	}

	using basic_state_visitor::operator();

private:
	after_comma_state* s_;
	const lex::token& token_;
};

state_result after_comma_state::process(parser& /*p*/,
										const lex::token& token) {
	if (mode_ == mode::post_sub)
		return state_result::push<expect_from_state>(builder_).reprocess();
	return token.visit(visitor{this, token.location(), token});
}

accept_result after_comma_state::accept_child(std::unique_ptr<ast::ast_node>) {
	if (mode_ == mode::awaiting_sub) {
		mode_ = mode::post_sub;
		return accept_result::stay();
	}
	return accept_result::complete(nullptr);
}
