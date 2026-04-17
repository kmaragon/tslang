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
 * \brief Resolves the `type` ambiguity in a specifier head
 *
 * Pushed by `specifier_head_state` after a `type` token was seen. The
 * coordinator holds the `type` token provisionally in `type_keyword_`.
 * On a terminator (`as`/`,`/`}`), `type` was the specifier name — move
 * it into `name_`. On a name-capable token, `type` was a modifier — the
 * current token becomes the name.
 */
class specifier_resolve_type_state : public parser_state {
public:
	explicit specifier_resolve_type_state(specifier_state* coordinator);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	specifier_state* coordinator_;

	class visitor;
};

}  // namespace tscc::parse::state
