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
#include "../type_node.hpp"

namespace tscc::parse::ast {

/**
 * \brief AST node for array type expressions (`T[]`)
 */
class array_type_node final : public type_node {
public:
	array_type_node(std::unique_ptr<const type_node> element,
					lex::token open_bracket,
					lex::token close_bracket);

	/**
	 * \brief Get the element type
	 */
	[[nodiscard]] const type_node& element_type() const noexcept;

private:
	std::unique_ptr<const type_node> element_;
	lex::token open_bracket_;
	lex::token close_bracket_;
};

}  // namespace tscc::parse::ast
