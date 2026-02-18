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

#include "../ast/import_node.hpp"
#include "parser_state.hpp"

namespace tscc::parse {

/**
 * \brief Coordinator state for all forms of TypeScript import declarations
 *
 * Entered when module_scope_state encounters an import keyword.
 * Immediately pushes after_import_state with reprocess; the chain of
 * per-phase states handles the grammar. When the chain completes,
 * accept_child returns the finished import_node.
 */
class import_state : public parser_state {
public:
	/**
	 * \brief Construct from the consumed import keyword token
	 */
	explicit import_state(lex::token import_keyword);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	std::unique_ptr<ast::import_node> node_;
};

}  // namespace tscc::parse
