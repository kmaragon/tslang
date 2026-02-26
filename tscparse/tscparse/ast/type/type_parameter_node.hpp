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
#include "../lexeme.hpp"
#include "../type_node.hpp"

namespace tscc::parse::state {
class type_parameter_state;
}

namespace tscc::parse::ast {

/**
 * \brief AST node for a generic type parameter
 *
 * Represents a single type parameter in a type parameter list,
 * e.g. `T`, `T extends U`, `T = Default`, `in T`, `out T`, `in out T`.
 * Shared across type aliases, interfaces, classes, and function declarations.
 */
class type_parameter_node final : public ast_node {
	friend class ::tscc::parse::state::type_parameter_state;

public:
	explicit type_parameter_node(lex::token name);

	/**
	 * \brief Get the parameter name as a lexeme
	 */
	[[nodiscard]] lexeme<std::string_view> name() const;

	/**
	 * \brief Whether the `in` variance modifier is present
	 */
	[[nodiscard]] bool has_in() const noexcept;

	/**
	 * \brief Whether the `out` variance modifier is present
	 */
	[[nodiscard]] bool has_out() const noexcept;

	/**
	 * \brief Get the constraint type (after `extends`), or nullptr if absent
	 */
	[[nodiscard]] const type_node* constraint() const noexcept;

	/**
	 * \brief Get the default type (after `=`), or nullptr if absent
	 */
	[[nodiscard]] const type_node* default_type() const noexcept;

private:

	lex::token in_keyword_;
	lex::token out_keyword_;
	lex::token name_;
	lex::token extends_;
	std::unique_ptr<const type_node> constraint_;
	lex::token equals_;
	std::unique_ptr<const type_node> default_;
};

}  // namespace tscc::parse::ast
