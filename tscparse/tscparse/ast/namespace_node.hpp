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
#include "module_node.hpp"
#include "qualified_name.hpp"

namespace tscc::parse::state {
class namespace_state;
class namespace_header_state;
}  // namespace tscc::parse::state

namespace tscc::parse::ast {

/**
 * \brief AST node for namespace/module declarations with identifier names
 *
 * Covers all four entry paths:
 * - `namespace Foo { ... }`
 * - `module Foo { ... }`
 * - `declare namespace Foo { ... }`
 * - `declare module Foo { ... }`
 *
 * Supports dotted names (e.g. `namespace A.B.C { ... }`).
 * Inherits the children vector from module_node.
 */
class namespace_node final : public module_node {
	friend class ::tscc::parse::state::namespace_state;
	friend class ::tscc::parse::state::namespace_header_state;

public:
	/**
	 * \brief Get the `declare` keyword token, if present
	 * \return Pointer to the token, or nullptr if no `declare` prefix
	 */
	[[nodiscard]] const lex::token* declare_keyword() const noexcept;

	/**
	 * \brief Get the `namespace` or `module` keyword token
	 */
	[[nodiscard]] const lex::token* keyword() const noexcept;

	/**
	 * \brief Get the namespace name (possibly dotted)
	 */
	[[nodiscard]] const qualified_name& name() const noexcept;

	/**
	 * \brief Whether this is an ambient declaration (`declare` prefix present)
	 */
	[[nodiscard]] bool ambient() const noexcept;

private:
	explicit namespace_node(lex::token keyword);

	lex::token declare_keyword_;
	lex::token keyword_;
	qualified_name name_;
	lex::token open_brace_;
	lex::token close_brace_;
};

}  // namespace tscc::parse::ast
