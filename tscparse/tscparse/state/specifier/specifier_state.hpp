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

class specifier_list_state;
class specifier_head_state;
class specifier_resolve_type_state;
class specifier_alias_state;

/**
 * \brief Coordinator for parsing a single `[type] name [as alias]` specifier
 *
 * Pushes `specifier_head_state` on first token to handle the `[type] name`
 * head sequence, then either completes the specifier on a `,`/`}` terminator
 * or pushes `specifier_alias_state` for the alias on `as`. Aggregates the
 * name, type keyword, and alias into the parent `specifier_list_state` via
 * `store_specifier()`.
 */
class specifier_state : public parser_state {
public:
	explicit specifier_state(specifier_list_state* list);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	friend class specifier_head_state;
	friend class specifier_resolve_type_state;
	friend class specifier_alias_state;

	specifier_list_state* list_;
	bool head_done_ = false;
	lex::token name_;
	lex::token type_keyword_;
	lex::token alias_;

	state_result flush_and_complete();
};

}  // namespace tscc::parse::state
