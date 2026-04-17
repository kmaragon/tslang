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

#include "export_assignment_state.hpp"
#include <tsclex/tokens/close_brace_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/semicolon_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"

using namespace tscc::parse::state;

export_assignment_state::export_assignment_state(lex::token equals_token)
	: node_(std::make_unique<ast::export_assignment_node>(
		  std::move(equals_token))) {}

state_result export_assignment_state::process_content(
	parser& /*p*/, const lex::token& token) {
	if (!has_identifier_) {
		if (!token.is<lex::tokens::identifier_token>())
			throw expected_token(token.location(), "'identifier'",
								 token->to_string());
		node_->identifier_ = token;
		has_identifier_ = true;
		return state_result::stay();
	}

	if (token.is<lex::tokens::semicolon_token>()) {
		node_->semicolon_ = token;
		return state_result::complete(std::move(node_));
	}

	// ASI: non-semicolon after identifier — complete and reprocess
	return state_result::complete(std::move(node_)).reprocess();
}

accept_result export_assignment_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	return accept_result::stay();
}

std::optional<state_result> export_assignment_state::on_eof() {
	if (has_identifier_)
		return state_result::complete(std::move(node_));
	return std::nullopt;
}
