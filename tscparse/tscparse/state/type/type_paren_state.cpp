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

#include "type_paren_state.hpp"
#include <tsclex/tokens/close_paren_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "type_expression_state.hpp"

using namespace tscc::parse::state;

type_paren_state::type_paren_state(lex::token open_paren,
								   ast::type_context ctx)
	: open_paren_(std::move(open_paren)), ctx_(ctx) {}

state_result type_paren_state::process(parser& /*p*/,
									   const lex::token& token) {
	if (!init_done_) {
		init_done_ = true;
		return state_result::push<type_expression_state>(ctx_).reprocess();
	}

	if (token.is<lex::tokens::close_paren_token>()) {
		auto* raw = const_cast<ast::type_definition*>(inner_.release());
		return state_result::complete(
			std::unique_ptr<ast::ast_node>(raw));
	}

	throw expected_token(token.location(), "')'", token->to_string());
}

accept_result type_paren_state::accept_child(
	std::unique_ptr<ast::ast_node> child) {
	inner_ = std::unique_ptr<const ast::type_definition>(
		static_cast<const ast::type_definition*>(child.release()));
	return accept_result::stay();
}
