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
 * \brief Expects the `as` keyword after `*` in a namespace import
 *
 * On seeing `as`, pushes expect_namespace_name_state.
 */
class expect_as_state : public parser_state {
public:
	explicit expect_as_state(import_node_builder* builder);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	import_node_builder* builder_;

	class visitor;
};

/**
 * \brief Expects an identifier for the namespace binding after `* as`
 *
 * On seeing an identifier, commits the namespace binding and pushes
 * expect_from_state.
 */
class expect_namespace_name_state : public parser_state {
public:
	explicit expect_namespace_name_state(import_node_builder* builder);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	import_node_builder* builder_;

	class visitor;
};

}  // namespace tscc::parse::state
