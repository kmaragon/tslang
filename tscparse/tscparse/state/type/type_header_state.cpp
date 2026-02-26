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

#include "type_header_state.hpp"
#include <tsclex/tokens/eq_token.hpp>
#include <tsclex/tokens/less_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "../token_helpers.hpp"
#include "type_parameter_list_state.hpp"

using namespace tscc;
using namespace tscc::parse::state;

type_header_state::type_header_state(ast::type_alias_node* node)
	: node_(node) {}

state_result type_header_state::process(parser& /*p*/,
										const lex::token& token) {
	if (!has_name_) {
		if (!detail::can_be_identifier(token)) {
			throw expected_token(token.location(), "identifier",
								 token->to_string());
		}
		lex::token normalized = token;
		detail::normalize_identifier(normalized);
		node_->name_ = std::move(normalized);
		has_name_ = true;
		return state_result::stay();
	}

	if (!type_params_done_) {
		if (token.is<lex::tokens::less_token>()) {
			type_params_done_ = true;
			node_->less_than_ = token;
			return state_result::push<type_parameter_list_state>(node_);
		}
		type_params_done_ = true;
	}

	if (token.is<lex::tokens::eq_token>()) {
		node_->equals_ = token;
		return state_result::complete(nullptr);
	}

	throw expected_token(token.location(), "'='", token->to_string());
}

accept_result type_header_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	// type_parameter_list_state completes with nullptr after storing
	// params directly on the node.
	return accept_result::stay();
}
