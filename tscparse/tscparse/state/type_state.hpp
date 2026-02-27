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
#include "../ast/type_node.hpp"
#include "parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Coordinator state for type alias declarations
 *
 * Handles both bare `type Foo = T;` and declared `declare type Foo = T;`.
 * Pushes type_header_state for the name, type params, and `=`,
 * then type_expression_state for the RHS type, then expects `;` or ASI.
 */
class type_state : public parser_state {
public:
	/**
	 * \brief Construct for bare type alias (no declare prefix)
	 *
	 * \param type_keyword The `type` keyword token
	 */
	explicit type_state(lex::token type_keyword);

	/**
	 * \brief Construct for declared type alias (always ambient)
	 *
	 * \param declare_keyword The `declare` keyword token
	 * \param type_keyword The `type` keyword token
	 */
	type_state(lex::token declare_keyword, lex::token type_keyword);

	state_result process(parser& p, const lex::token& token) override;

	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

	std::optional<state_result> on_eof() override;

private:
	std::unique_ptr<ast::type_node> node_;
	bool header_done_ = false;
	bool type_done_ = false;
};

}  // namespace tscc::parse::state
