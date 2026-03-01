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

#include "type_reference_state.hpp"
#include <tsclex/tokens/dot_token.hpp>
#include <tsclex/tokens/newline_token.hpp>
#include <tsclex/tokens/less_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "../token_helpers.hpp"
#include "type_argument_list_state.hpp"

using namespace tscc;
using namespace tscc::parse::state;

type_reference_state::type_reference_state(ast::type_context ctx)
	: ctx_(ctx), node_(std::make_unique<ast::type_reference_node>()) {}

state_result type_reference_state::process(parser& /*p*/,
										   const lex::token& token) {
	if (token.is<lex::tokens::newline_token>()) return state_result::stay();

	if (expect_segment_) {
		if (!detail::can_be_identifier(token)) {
			throw expected_token(token.location(), "identifier",
								 token->to_string());
		}
		lex::token normalized = token;
		detail::normalize_identifier(normalized);
		node_->name_.segments_.emplace_back(std::move(normalized));
		expect_segment_ = false;
		return state_result::stay();
	}

	if (!type_args_done_) {
		if (token.is<lex::tokens::dot_token>()) {
			expect_segment_ = true;
			return state_result::stay();
		}

		if (token.is<lex::tokens::less_token>()) {
			type_args_done_ = true;
			node_->less_than_ = token;
			return state_result::push<type_argument_list_state>(
				node_.get(), ctx_);
		}
	}

	return state_result::complete(std::move(node_)).reprocess();
}

accept_result type_reference_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	// type_argument_list_state completes with nullptr after populating
	// the node's type arguments directly.
	type_args_done_ = true;
	return accept_result::complete(std::move(node_));
}
