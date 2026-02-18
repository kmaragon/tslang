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
#include <tsclex/tokens/as_token.hpp>
#include <tsclex/tokens/assert_token.hpp>
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/default_token.hpp>
#include <tsclex/tokens/from_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/require_token.hpp>
#include <tsclex/tokens/type_token.hpp>

namespace tscc::parse::import_helpers {

/**
 * \brief Check if a token can serve as an identifier in import context
 *
 * TypeScript allows contextual keywords as identifiers in binding
 * positions. This accepts identifier_token and all non-reserved keywords.
 */
inline bool can_be_identifier(const lex::token& token) {
	struct visitor {
		bool operator()(const lex::tokens::identifier_token&) const {
			return true;
		}
		bool operator()(const lex::tokens::type_token&) const { return true; }
		bool operator()(const lex::tokens::from_token&) const { return true; }
		bool operator()(const lex::tokens::as_token&) const { return true; }
		bool operator()(const lex::tokens::assert_token&) const { return true; }
		bool operator()(const lex::tokens::require_token&) const {
			return true;
		}
		bool operator()(const lex::tokens::basic_token&) const { return false; }
	};
	return token.visit(visitor{});
}

/**
 * \brief Check if a token can serve as a named import specifier name
 *
 * In addition to identifiers, named imports accept string literals
 * and the default keyword: import { "string-name" as x } or
 * import { default as x }.
 */
inline bool can_be_specifier_name(const lex::token& token) {
	return can_be_identifier(token) ||
		   token.is<lex::tokens::constant_value_token>() ||
		   token.is<lex::tokens::default_token>();
}

}  // namespace tscc::parse::import_helpers
