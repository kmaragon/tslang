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

namespace tscc::parse::state {
class specifier_list_state;
class import_node_builder;
class named_export_state;
}

namespace tscc::parse::ast {

/**
 * \brief A single specifier within a named import or export list
 *
 * Represents one element inside braces: [type] name [as alias]
 *
 * The name field holds an identifier_token (contextual keywords are
 * normalized), a constant_value_token (string literal), or a
 * default_token. The alias, when present, is always an identifier_token.
 */
class named_specifier {
	friend class state::specifier_list_state;
	friend class state::import_node_builder;
	friend class state::named_export_state;

	lex::token name_;
	lex::token type_keyword_;
	lex::token alias_;

public:
	/**
	 * \brief Get the specifier name as a lexeme
	 *
	 * Returns an invalid lexeme when the name is `default` —
	 * use is_default() for that case.
	 */
	[[nodiscard]] lexeme<std::string_view> name() const;

	/**
	 * \brief Whether this specifier uses the `default` keyword as its name
	 */
	[[nodiscard]] bool is_default() const noexcept;

	/**
	 * \brief Get the type-only modifier keyword, if present
	 */
	[[nodiscard]] const lex::token* type_keyword() const noexcept;

	/**
	 * \brief Get the alias binding after `as`, if present
	 */
	[[nodiscard]] lexeme<std::string_view> alias() const;
};

}  // namespace tscc::parse::ast
