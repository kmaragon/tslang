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
#include "ast_node.hpp"

namespace tscc::parse::ast {

/**
 * \brief Base class for AST nodes that can carry an `export` keyword
 *
 * Provides the common `export_keyword` storage and accessor shared by
 * all declaration node types that may be prefixed with `export`.
 */
class exportable_node : public ast_node {
public:
	/**
	 * \brief Get the `export` keyword token, if present
	 * \return Pointer to the token, or nullptr if no `export` prefix
	 */
	[[nodiscard]] const lex::token* export_keyword() const noexcept;

	/**
	 * \brief Set the `export` keyword token
	 *
	 * Called by parser states during construction. Treat as const
	 * after the owning state completes.
	 */
	void set_export_keyword(lex::token token) noexcept;

private:
	lex::token export_keyword_;
};

}  // namespace tscc::parse::ast
