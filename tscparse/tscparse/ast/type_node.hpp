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
#include <vector>
#include <tsclex/token.hpp>
#include "exportable_node.hpp"
#include "lexeme.hpp"
#include "type/type_definition.hpp"
#include "type/type_parameter_node.hpp"

namespace tscc::parse::state {
class type_state;
class type_header_state;
class type_parameter_list_state;
}

namespace tscc::parse::ast {

/**
 * \brief AST node for type alias declarations
 *
 * Represents `type Foo<T> = SomeType;` declarations.
 * Supports optional `declare` prefix and optional type parameters.
 */
class type_node final : public exportable_node {
	friend class state::type_state;
	friend class state::type_header_state;
	friend class state::type_parameter_list_state;

public:
	explicit type_node(lex::token type_keyword);

	kind node_kind() const noexcept override { return kind::type; }

	/**
	 * \brief Get the `declare` keyword token, if present
	 * \return Pointer to the token, or nullptr if no `declare` prefix
	 */
	[[nodiscard]] const lex::token* declare_keyword() const noexcept;

	/**
	 * \brief Get the `type` keyword token
	 */
	[[nodiscard]] const lex::token* keyword() const noexcept;

	/**
	 * \brief Get the alias name as a lexeme
	 */
	[[nodiscard]] lexeme<std::string_view> name() const;

	/**
	 * \brief Get the type parameters
	 */
	[[nodiscard]] const std::vector<std::unique_ptr<const type_parameter_node>>&
	type_parameters() const noexcept;

	/**
	 * \brief Get the RHS type expression
	 */
	[[nodiscard]] const type_definition& type() const noexcept;

	/**
	 * \brief Whether this is an ambient declaration (`declare` prefix present)
	 */
	[[nodiscard]] bool ambient() const noexcept;

private:
	lex::token declare_keyword_;
	lex::token type_keyword_;
	lex::token name_;
	lex::token less_than_;
	std::vector<std::unique_ptr<const type_parameter_node>> type_parameters_;
	lex::token greater_than_;
	lex::token equals_;
	std::unique_ptr<const type_definition> type_;
	lex::token semicolon_;
};

}  // namespace tscc::parse::ast
