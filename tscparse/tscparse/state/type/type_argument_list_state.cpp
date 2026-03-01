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

#include "type_argument_list_state.hpp"
#include <tsclex/tokens/comma_token.hpp>
#include <tsclex/tokens/newline_token.hpp>
#include <tsclex/tokens/greater_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "type_expression_state.hpp"

using namespace tscc;
using namespace tscc::parse::state;

type_argument_list_state::type_argument_list_state(
	ast::type_reference_node* node, ast::type_context ctx)
	: node_(node), ctx_(ctx) {}

state_result type_argument_list_state::process(parser& /*p*/,
											   const lex::token& token) {
	if (token.is<lex::tokens::newline_token>()) return state_result::stay();

	if (!init_done_) {
		init_done_ = true;
		return state_result::push<type_expression_state>(ctx_).reprocess();
	}

	if (token.is<lex::tokens::comma_token>()) {
		return state_result::push<type_expression_state>(ctx_);
	}

	if (token.is<lex::tokens::greater_token>()) {
		node_->greater_than_ = token;
		return state_result::complete(nullptr);
	}

	// Phase 2: handle double_greater_token (>>) for nested generics.
	throw expected_token(token.location(), "',' or '>'", token->to_string());
}

accept_result type_argument_list_state::accept_child(
	std::unique_ptr<ast::ast_node> child) {
	node_->type_arguments_.emplace_back(
		std::unique_ptr<const ast::type_definition>(
			static_cast<const ast::type_definition*>(child.release())));
	return accept_result::stay();
}
