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

#include <vector>
#include <tsclex/token.hpp>
#include "lexeme.hpp"

namespace tscc::parse::state {
class import_node_builder;
class namespace_header_state;
}

namespace tscc::parse::ast {

/**
 * \brief A dot-separated sequence of identifiers
 *
 * Represents qualified names such as A.B.C in import-equals entity
 * names, type references, and dotted namespace declarations.
 * Each segment is a normalized identifier token.
 */
class qualified_name {
	friend class state::import_node_builder;
	friend class state::namespace_header_state;

public:
	/// Whether the name has no segments
	[[nodiscard]] bool empty() const noexcept;

	/// Number of segments
	[[nodiscard]] size_t size() const noexcept;

	/// Whether the name has any segments
	operator bool() const noexcept;

	/// Access the i-th segment as a lexeme
	lexeme<std::string_view> operator[](size_t i) const;

	/// Raw token access for iteration and type inspection
	[[nodiscard]] const std::vector<lex::token>& tokens() const noexcept;

private:
	std::vector<lex::token> segments_;
};

}  // namespace tscc::parse::ast
