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

#include "../ast/module_node.hpp"
#include "parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Root state for parsing at module/file scope
 *
 * This is the initial state when parsing a TypeScript file.
 * It handles top-level declarations: imports, exports, classes,
 * functions, interfaces, enums, type aliases, and variable declarations.
 *
 * Completed child nodes are integrated into the target module_node
 * via accept_child.
 */
class module_scope_state : public parser_state {
public:
	explicit module_scope_state(ast::module_node* target);

	state_result process(parser& p, const lex::token& token) override;

	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	ast::module_node* target_;
};

}  // namespace tscc::parse::state
