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
#include "../../ast/export_assignment_node.hpp"
#include "../multiline_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Parser state for `export = identifier ;`
 *
 * Entered after `export` has been consumed and `=` has been seen.
 * Expects an identifier token, then a semicolon (or ASI via newline/EOF).
 */
class export_assignment_state : public multiline_state {
public:
	explicit export_assignment_state(lex::token equals_token);

	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

	std::optional<state_result> on_eof() override;

protected:
	state_result process_content(parser& p, const lex::token& token) override;

private:
	std::unique_ptr<ast::export_assignment_node> node_;
	bool has_identifier_ = false;
};

}  // namespace tscc::parse::state
