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
#include "../qualified_name.hpp"
#include "type_definition.hpp"

namespace tscc::parse::state {
class type_reference_state;
class type_argument_list_state;
}

namespace tscc::parse::ast {

/**
 * \brief AST node for type references (`Foo`, `A.B.C`, `Map<K, V>`)
 *
 * Covers named type references with optional dot-separated qualifiers
 * and optional type arguments.
 */
class type_reference_node final : public type_definition {
	friend class ::tscc::parse::state::type_reference_state;
	friend class ::tscc::parse::state::type_argument_list_state;

public:
	type_reference_node();

	type_definition::kind type_kind() const noexcept override {
		return kind::reference;
	}

	/**
	 * \brief Get the qualified name
	 */
	[[nodiscard]] const qualified_name& name() const noexcept;

	/**
	 * \brief Get the type arguments (empty if no angle brackets)
	 */
	[[nodiscard]] const std::vector<std::unique_ptr<const type_definition>>&
	type_arguments() const noexcept;

private:

	qualified_name name_;
	lex::token less_than_;
	std::vector<std::unique_ptr<const type_definition>> type_arguments_;
	lex::token greater_than_;
};

}  // namespace tscc::parse::ast
