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
#include "../type_node.hpp"

namespace tscc::parse::ast {

/**
 * \brief AST node for literal type expressions
 *
 * Covers string literals ("foo"), numeric literals (42, -1),
 * and boolean literals (true, false).
 */
class literal_type_node final : public type_node {
public:
	explicit literal_type_node(lex::token value);
	literal_type_node(lex::token minus, lex::token value);

	/**
	 * \brief Get the optional minus token (for negative numeric literals)
	 */
	[[nodiscard]] const lex::token* minus_token() const noexcept;

	/**
	 * \brief Get the value token (string, number, true, or false)
	 */
	[[nodiscard]] const lex::token& value_token() const noexcept;

private:
	lex::token minus_;
	lex::token value_;
};

}  // namespace tscc::parse::ast
