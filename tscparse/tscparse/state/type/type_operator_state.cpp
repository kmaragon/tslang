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

#include "type_operator_state.hpp"
#include "../state_result.hpp"
#include "type_postfix_state.hpp"

using namespace tscc::parse::state;

type_operator_state::type_operator_state(ast::type_context ctx) : ctx_(ctx) {}

state_result type_operator_state::process(parser& /*p*/,
										  const lex::token& /*token*/) {
	// Phase 2: handle keyof, readonly, unique symbol here.
	// For now, pass through to postfix level.
	return state_result::push<type_postfix_state>(ctx_).reprocess();
}

accept_result type_operator_state::accept_child(
	std::unique_ptr<ast::ast_node> child) {
	return accept_result::complete(std::move(child));
}
