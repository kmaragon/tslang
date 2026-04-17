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

#include "specifier_head_state.hpp"
#include <tsclex/tokens/as_token.hpp>
#include <tsclex/tokens/assert_token.hpp>
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/default_token.hpp>
#include <tsclex/tokens/from_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/require_token.hpp>
#include <tsclex/tokens/type_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "specifier_resolve_type_state.hpp"
#include "specifier_state.hpp"

using namespace tscc::parse::state;

class specifier_head_state::visitor : public basic_state_visitor {
public:
	visitor(specifier_head_state* s, const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::type_token&) const {
		s_->coordinator_->type_keyword_ = token_;
		return state_result::push<specifier_resolve_type_state>(
			s_->coordinator_);
	}

	state_result operator()(const lex::tokens::identifier_token&) const {
		return handle_name();
	}
	state_result operator()(const lex::tokens::from_token&) const {
		return handle_name();
	}
	state_result operator()(const lex::tokens::as_token&) const {
		return handle_name();
	}
	state_result operator()(const lex::tokens::assert_token&) const {
		return handle_name();
	}
	state_result operator()(const lex::tokens::require_token&) const {
		return handle_name();
	}
	state_result operator()(const lex::tokens::constant_value_token&) const {
		return handle_name();
	}
	state_result operator()(const lex::tokens::default_token&) const {
		return handle_name();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "specifier or '}'",
							 token_->to_string());
	}

	using basic_state_visitor::operator();

private:
	state_result handle_name() const {
		s_->coordinator_->name_ = token_;
		return state_result::complete(nullptr);
	}

	specifier_head_state* s_;
	const lex::token& token_;
};

specifier_head_state::specifier_head_state(specifier_state* coordinator)
	: coordinator_(coordinator) {}

state_result specifier_head_state::process(parser& /*p*/,
										   const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result specifier_head_state::accept_child(
	std::unique_ptr<ast::ast_node>) {
	return accept_result::complete(nullptr);
}
