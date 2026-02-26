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
 * \brief AST node for keyword type expressions
 *
 * Covers the built-in keyword types: string, number, boolean, void,
 * never, unknown, any, object, symbol, bigint, undefined, null.
 */
class keyword_type_node final : public type_node {
public:
	explicit keyword_type_node(lex::token keyword);

	/**
	 * \brief Get the keyword token
	 */
	[[nodiscard]] const lex::token& keyword() const noexcept;

private:
	lex::token keyword_;
};

}  // namespace tscc::parse::ast
