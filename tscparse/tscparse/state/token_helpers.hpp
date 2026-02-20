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

namespace tscc::parse::state::detail {

/**
 * \brief Check if a token can serve as an identifier
 *
 * TypeScript allows contextual keywords as identifiers in many
 * positions. This accepts identifier_token and all non-reserved keywords
 * that can appear where an identifier is expected.
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

/**
 * \brief Normalize a contextual keyword token to an identifier_token
 *
 * If the token is a contextual keyword that can serve as an identifier
 * (type, from, as, assert, require) but is not already an identifier_token,
 * replaces its variant payload in place with an identifier_token whose
 * name is the keyword text. Source location is preserved.
 *
 * No-op if the token is already an identifier_token or is not a
 * contextual keyword.
 */
inline void normalize_identifier(lex::token& tok) {
	if (tok.is<lex::tokens::identifier_token>()) return;
	if (!can_be_identifier(tok)) return;
	auto name = tok->to_string();
	std::u32string u32name(name.begin(), name.end());
	tok.emplace_token<lex::tokens::identifier_token>(tok.location(), u32name);
}

}  // namespace tscc::parse::state::detail
