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
#include "type_definition.hpp"

namespace tscc::parse::ast {

/**
 * \brief AST node for union type expressions (`A | B | C`)
 */
class union_type_node final : public type_definition {
public:
	explicit union_type_node(
		std::vector<std::unique_ptr<const type_definition>> members);

	type_definition::kind type_kind() const noexcept override {
		return kind::union_kind;
	}

	/**
	 * \brief Get the member types (2 or more)
	 */
	[[nodiscard]] const std::vector<std::unique_ptr<const type_definition>>&
	members() const noexcept;

private:
	std::vector<std::unique_ptr<const type_definition>> members_;
};

}  // namespace tscc::parse::ast
