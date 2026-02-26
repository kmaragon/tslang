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

#pragma once

#include <memory>
#include <tsclex/token.hpp>
#include "../../ast/type/type_context.hpp"
#include "../../ast/type_node.hpp"
#include "../parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Parses parenthesized type expressions `(T)`
 *
 * Consumes `(`, parses the inner type expression, expects `)`, and
 * completes with the inner type directly (no wrapper node).
 * Phase 3 will add disambiguation with function types `(a: A) => B`.
 */
class type_paren_state : public parser_state {
public:
	type_paren_state(lex::token open_paren, ast::type_context ctx);

	state_result process(parser& p, const lex::token& token) override;

	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	lex::token open_paren_;
	ast::type_context ctx_;
	bool init_done_ = false;
	std::unique_ptr<const ast::type_node> inner_;
};

}  // namespace tscc::parse::state
