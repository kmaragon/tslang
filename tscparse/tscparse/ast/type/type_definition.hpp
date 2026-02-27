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

#include <cstdint>
#include "../ast_node.hpp"

namespace tscc::parse::ast {

/**
 * \brief Abstract base class for all type expression nodes
 *
 * Provides compile-time safety for type positions: any AST slot that
 * expects a type expression uses unique_ptr<const type_definition>
 * rather than bare ast_node, catching misuse at compile time.
 */
class type_definition : public ast_node {
public:
	/**
	 * \brief Discriminant for type expression dispatch
	 */
	enum class kind : uint8_t {
		keyword,
		literal,
		this_kind,
		reference,
		array,
		union_kind,
		intersection,
	};

	~type_definition() override = default;

	ast_node::kind node_kind() const noexcept final {
		return ast_node::kind::type_definition;
	}

	/**
	 * \brief Get the type expression kind
	 */
	[[nodiscard]] virtual type_definition::kind type_kind() const noexcept = 0;

protected:
	type_definition() = default;
};

}  // namespace tscc::parse::ast
