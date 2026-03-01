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

#include <tsclex/token.hpp>
#include "parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Routing state for the `export` keyword
 *
 * Dispatches on the token following `export` to push the appropriate
 * declaration state. When the child completes, stamps the export keyword
 * on the returned AST node.
 */
class export_state : public parser_state {
public:
	explicit export_state(lex::token export_keyword);

	state_result process(parser& p, const lex::token& token) override;

	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	lex::token export_keyword_;
};

}  // namespace tscc::parse::state
