/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2025. Keef Aragon
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
#include "filtered_token_index.hpp"

namespace tscc::parse {

// Forward declaration
namespace ast {
class ast_node;
}

/**
 * \brief Reference to a trivia token with context
 */
struct trivia_ref {
	lex::token token;  // Owns the token
	const ast::ast_node* associated_node;

	enum class relationship {
		leading,   // Before the node
		trailing,  // After on same line
		orphaned   // Not attached to any node
	} rel;

	const lex::source_location& location() const { return token.location(); }
};

/**
 * \brief Sorted index of trivia (comments) for efficient location-based lookup
 */
using trivia_index = filtered_token_index<trivia_ref,
										  lex::tokens::comment_token,
										  lex::tokens::multiline_comment_token,
										  lex::tokens::jsdoc_token>;

}  // namespace tscc::parse