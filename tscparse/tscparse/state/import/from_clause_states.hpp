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

#include "../parser_state.hpp"
#include "import_node_builder.hpp"

namespace tscc::parse::state {

/**
 * \brief Expects the `from` keyword in a from-style import
 *
 * Pushed after default binding, namespace binding, or named imports.
 * On seeing `from`, sets it on the node and pushes after_from_state.
 */
class expect_from_state : public parser_state {
public:
	explicit expect_from_state(import_node_builder* builder);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	import_node_builder* builder_;

	class visitor;
};

/**
 * \brief Expects a string literal module specifier after `from`
 *
 * On seeing a string literal, sets the module specifier and pushes
 * after_module_spec_state.
 */
class after_from_state : public parser_state {
public:
	explicit after_from_state(import_node_builder* builder);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	import_node_builder* builder_;

	class visitor;
};

}  // namespace tscc::parse::state
