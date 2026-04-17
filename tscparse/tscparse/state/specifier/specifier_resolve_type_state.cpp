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

#include "specifier_resolve_type_state.hpp"
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
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "specifier_state.hpp"

using namespace tscc::parse::state;

class specifier_resolve_type_state::visitor : public basic_state_visitor {
public:
	visitor(specifier_resolve_type_state* s, const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::as_token&) const {
		return demote_type_to_name_with_reprocess();
	}
	state_result operator()(const lex::tokens::comma_token&) const {
		return demote_type_to_name_with_reprocess();
	}
	state_result operator()(const lex::tokens::close_brace_token&) const {
		return demote_type_to_name_with_reprocess();
	}

	state_result operator()(const lex::tokens::identifier_token&) const {
		return adopt_as_name();
	}
	state_result operator()(const lex::tokens::from_token&) const {
		return adopt_as_name();
	}
	state_result operator()(const lex::tokens::assert_token&) const {
		return adopt_as_name();
	}
	state_result operator()(const lex::tokens::require_token&) const {
		return adopt_as_name();
	}
	state_result operator()(const lex::tokens::constant_value_token&) const {
		return adopt_as_name();
	}
	state_result operator()(const lex::tokens::default_token&) const {
		return adopt_as_name();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "specifier name, 'as', ',', or '}'",
							 token_->to_string());
	}

	using basic_state_visitor::operator();

private:
	state_result demote_type_to_name_with_reprocess() const {
		s_->coordinator_->name_ = std::move(s_->coordinator_->type_keyword_);
		s_->coordinator_->type_keyword_ = lex::token{};
		return state_result::complete(nullptr).reprocess();
	}

	state_result adopt_as_name() const {
		s_->coordinator_->name_ = token_;
		return state_result::complete(nullptr);
	}

	specifier_resolve_type_state* s_;
	const lex::token& token_;
};

specifier_resolve_type_state::specifier_resolve_type_state(
	specifier_state* coordinator)
	: coordinator_(coordinator) {}

state_result specifier_resolve_type_state::process(parser& /*p*/,
												   const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result specifier_resolve_type_state::accept_child(
	std::unique_ptr<ast::ast_node>) {
	throw std::logic_error(
		"specifier_resolve_type_state does not push children");
}
