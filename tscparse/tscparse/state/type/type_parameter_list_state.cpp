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

#include "type_parameter_list_state.hpp"
#include <tsclex/tokens/comma_token.hpp>
#include <tsclex/tokens/greater_token.hpp>
#include "../../ast/type/type_parameter_node.hpp"
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "type_parameter_state.hpp"

using namespace tscc;
using namespace tscc::parse::state;

type_parameter_list_state::type_parameter_list_state(
	ast::type_alias_node* node)
	: node_(node) {}

state_result type_parameter_list_state::process(parser& /*p*/,
												const lex::token& token) {
	if (!init_done_) {
		init_done_ = true;
		if (token.is<lex::tokens::greater_token>()) {
			throw expected_token(token.location(), "type parameter",
								 token->to_string());
		}
		return state_result::push<type_parameter_state>().reprocess();
	}

	if (token.is<lex::tokens::comma_token>()) {
		return state_result::push<type_parameter_state>();
	}

	if (token.is<lex::tokens::greater_token>()) {
		node_->greater_than_ = token;
		return state_result::complete(nullptr);
	}

	throw expected_token(token.location(), "',' or '>'", token->to_string());
}

accept_result type_parameter_list_state::accept_child(
	std::unique_ptr<ast::ast_node> child) {
	node_->type_parameters_.emplace_back(
		std::unique_ptr<const ast::type_parameter_node>(
			static_cast<const ast::type_parameter_node*>(child.release())));
	return accept_result::stay();
}
