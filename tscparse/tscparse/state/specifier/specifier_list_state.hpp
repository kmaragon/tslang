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
#include "../parser_state.hpp"

namespace tscc::parse::state {

class specifier_state;

/**
 * \brief Base class for parsing a braced specifier list: { a, b as c, type d }
 *
 * Handles only the list-level grammar: opening was already consumed by
 * the caller, this state handles commas, the closing brace, and pushes
 * a `specifier_state` child for each specifier. Subclasses provide
 * storage via `store_specifier()` and list completion via
 * `on_list_complete()`.
 *
 * Used by both named imports and named exports.
 */
class specifier_list_state : public parser_state {
public:
	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

	/**
	 * \brief Store a parsed specifier into the underlying AST container
	 *
	 * Called by `specifier_state` when it completes a `[type] name [as
	 * alias]` sequence. Identifiers are already normalized by the child
	 * state before this is invoked.
	 */
	virtual void store_specifier(lex::token name, lex::token type_keyword,
								 lex::token alias) = 0;

protected:
	virtual state_result on_list_complete() = 0;

private:
	enum class phase {
		expect_specifier_or_close,
		expect_comma_or_close,
	};

	phase phase_ = phase::expect_specifier_or_close;
};

}  // namespace tscc::parse::state
