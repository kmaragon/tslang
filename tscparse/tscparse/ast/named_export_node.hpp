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
#include <vector>
#include "exportable_node.hpp"
#include "lexeme.hpp"
#include "named_specifier.hpp"

namespace tscc::parse::state {
class named_export_state;
}

namespace tscc::parse::ast {

/**
 * \brief AST node for `export { a, b as c }` and `export { a } from "module"`
 *
 * Represents a named export list with optional re-export source.
 * The `export` keyword is set via the inherited exportable_node interface.
 */
class named_export_node final : public exportable_node {
	friend class state::named_export_state;

public:
	kind node_kind() const noexcept override;

	/**
	 * \brief Get the export specifiers
	 */
	[[nodiscard]] const std::vector<named_specifier>& specifiers()
		const noexcept;

	/**
	 * \brief Get the module specifier for re-exports
	 *
	 * Returns an invalid lexeme when this is a local export (no `from`).
	 */
	[[nodiscard]] lexeme<std::string> module_specifier() const;

private:
	std::vector<named_specifier> specifiers_;
	lex::token module_specifier_;
	lex::token semicolon_;
};

}  // namespace tscc::parse::ast
