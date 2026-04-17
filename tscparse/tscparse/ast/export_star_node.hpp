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
class export_star_state;
}

namespace tscc::parse::ast {

/**
 * \brief AST node for `export * from "module"` and `export * as ns from "module"`
 *
 * Represents a star re-export declaration. The `export` keyword
 * is set via the inherited exportable_node interface.
 */
class export_star_node final : public exportable_node {
	friend class state::export_star_state;

public:
	explicit export_star_node(lex::token asterisk_token);

	kind node_kind() const noexcept override;

	/**
	 * \brief Get the `*` token
	 */
	[[nodiscard]] const lex::token* asterisk() const noexcept;

	/**
	 * \brief Get the namespace alias after `as`, if present
	 */
	[[nodiscard]] lexeme<std::string_view> namespace_name() const;

	/**
	 * \brief Get the module specifier string literal
	 */
	[[nodiscard]] lexeme<std::string> module_specifier() const;

private:
	lex::token asterisk_;
	lex::token as_name_;
	lex::token module_specifier_;
	lex::token semicolon_;
};

}  // namespace tscc::parse::ast
