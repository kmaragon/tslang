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

#include "type_expression_state.hpp"
#include "../state_result.hpp"
#include "type_union_state.hpp"

using namespace tscc::parse::state;

type_expression_state::type_expression_state(ast::type_context ctx)
	: ctx_(ctx) {}

state_result type_expression_state::process(parser& /*p*/,
											const lex::token& /*token*/) {
	if (!init_done_) {
		init_done_ = true;
		return state_result::push<type_union_state>(ctx_).reprocess();
	}

	// Phase 2 will check for `extends` here to handle conditionals.
	// For now, whatever token caused us to return here is not ours.
	throw std::logic_error(
		"type_expression_state::process called after init");
}

accept_result type_expression_state::accept_child(
	std::unique_ptr<ast::ast_node> child) {
	return accept_result::complete(std::move(child));
}
