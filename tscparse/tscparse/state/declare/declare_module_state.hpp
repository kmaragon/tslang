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
#include "../../ast/declare_module_node.hpp"
#include "../parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Coordinator state for `declare module "name" { ... }`
 *
 * Creates the declare_module_node and pushes a header sub-state to
 * consume the module name and opening brace. Once the header completes,
 * this state handles body tokens via module_scope_visitor, checking for
 * the closing brace.
 */
class declare_module_state : public parser_state {
public:
	declare_module_state(lex::token declare_keyword, lex::token module_keyword);

	state_result process(parser& p, const lex::token& token) override;

	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	std::unique_ptr<ast::declare_module_node> node_;
	bool header_done_ = false;
};

}  // namespace tscc::parse::state
