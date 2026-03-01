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
#include <vector>
#include "../../ast/type/type_context.hpp"
#include "../../ast/type/type_definition.hpp"
#include "../multiline_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Parses union type expressions (`A | B | C`)
 *
 * Handles the union precedence level. Pushes intersection level for
 * each operand. Supports leading `|` (e.g. `| A | B`).
 * If only one operand, returns it directly without wrapping.
 */
class type_union_state : public multiline_state {
public:
	explicit type_union_state(ast::type_context ctx);

	state_result process_content(parser& p, const lex::token& token) override;

	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

	std::optional<state_result> on_eof() override;

private:
	ast::type_context ctx_;
	bool init_done_ = false;
	std::vector<std::unique_ptr<const ast::type_definition>> members_;
};

}  // namespace tscc::parse::state
