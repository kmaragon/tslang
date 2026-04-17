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
#include "exportable_node.hpp"
#include "lexeme.hpp"

namespace tscc::parse::state {
class export_assignment_state;
}

namespace tscc::parse::ast {

/**
 * \brief AST node for `export = identifier;` declarations
 *
 * Represents a CommonJS-style export assignment. The `export` keyword
 * is set via the inherited exportable_node interface.
 */
class export_assignment_node final : public exportable_node {
	friend class state::export_assignment_state;

public:
	explicit export_assignment_node(lex::token equals_token);

	kind node_kind() const noexcept override;

	/**
	 * \brief Get the `=` token
	 */
	[[nodiscard]] const lex::token* equals_token() const noexcept;

	/**
	 * \brief Get the exported identifier as a lexeme
	 */
	[[nodiscard]] lexeme<std::string_view> identifier() const;

private:
	lex::token equals_;
	lex::token identifier_;
	lex::token semicolon_;
};

}  // namespace tscc::parse::ast
