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

#include <memory>
#include <tsclex/token.hpp>
#include "../../ast/type/type_context.hpp"
#include "../../ast/type/type_definition.hpp"
#include "../parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Handles postfix type operations (`T[]`, `T[K]`)
 *
 * Pushes primary level for the base type, then checks for `[` to
 * apply array or indexed access suffixes.
 */
class type_postfix_state : public parser_state {
public:
	explicit type_postfix_state(ast::type_context ctx);

	state_result process(parser& p, const lex::token& token) override;

	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

	std::optional<state_result> on_eof() override;

private:
	ast::type_context ctx_;
	bool init_done_ = false;
	std::unique_ptr<const ast::type_definition> base_;
	lex::token open_bracket_;
};

}  // namespace tscc::parse::state
