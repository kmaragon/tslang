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

#include "../../ast/type/type_alias_node.hpp"
#include "../parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Parses a type parameter list `<T, U extends V, W = Default>`
 *
 * Entered after the opening `<` has been consumed. Pushes
 * type_parameter_state for each parameter. Expects `,` between
 * parameters and `>` to close. Stores results directly into the
 * type_alias_node.
 */
class type_parameter_list_state : public parser_state {
public:
	explicit type_parameter_list_state(ast::type_alias_node* node);

	state_result process(parser& p, const lex::token& token) override;

	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	ast::type_alias_node* node_;
	bool init_done_ = false;
};

}  // namespace tscc::parse::state
