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

#include "../../ast/namespace_node.hpp"
#include "../parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Sub-state that consumes the namespace qualified name and opening brace
 *
 * Parses a dot-separated identifier sequence (e.g. `A.B.C`) followed by `{`.
 * Stores the segments into the namespace_node's qualified_name and the opening
 * brace, then completes with nullptr to hand control back to the coordinator.
 */
class namespace_header_state : public parser_state {
public:
	explicit namespace_header_state(ast::namespace_node* node);

	state_result process(parser& p, const lex::token& token) override;

	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	ast::namespace_node* node_;
	bool expect_segment_ = true;
};

}  // namespace tscc::parse::state
