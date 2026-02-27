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
#include "lexeme.hpp"
#include "module_node.hpp"

namespace tscc::parse::state {
class declare_module_state;
class declare_module_header_state;
}

namespace tscc::parse::ast {

/**
 * \brief AST node for `declare module "name" { ... }`
 *
 * Represents an ambient external module declaration. The body is a full
 * module scope where imports and exports are valid. Inherits the children
 * vector from module_node.
 */
class declare_module_node final : public module_node {
	friend class ::tscc::parse::state::declare_module_state;
	friend class ::tscc::parse::state::declare_module_header_state;

public:
	declare_module_node(lex::token declare_keyword, lex::token module_keyword);

	kind node_kind() const noexcept override { return kind::declare_module; }

	/**
	 * \brief Get the `declare` keyword token
	 */
	[[nodiscard]] const lex::token* declare_keyword() const noexcept;

	/**
	 * \brief Get the `module` keyword token
	 */
	[[nodiscard]] const lex::token* module_keyword() const noexcept;

	/**
	 * \brief Get the module name (string literal value)
	 */
	[[nodiscard]] lexeme<std::string> module_name() const;

private:

	lex::token declare_keyword_;
	lex::token module_keyword_;
	lex::token module_name_;
	lex::token open_brace_;
	lex::token close_brace_;
};

}  // namespace tscc::parse::ast
