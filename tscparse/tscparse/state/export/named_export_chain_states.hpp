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

#include "../parser_state.hpp"

namespace tscc::parse::state {

class named_export_state;

/**
 * \brief Position after `from` in a re-export — expects a string literal
 *
 * Stores the module specifier on the coordinator and pushes
 * named_export_semicolon_state.
 */
class named_export_module_spec_state : public parser_state {
public:
	explicit named_export_module_spec_state(named_export_state* coord);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	named_export_state* coord_;

	class visitor;
};

/**
 * \brief Terminator position for `export { ... } from "module"` — expects `;` or ASI
 *
 * On `;`: stores and completes. On other tokens: completes with reprocess.
 * On EOF: completes.
 */
class named_export_semicolon_state : public parser_state {
public:
	explicit named_export_semicolon_state(named_export_state* coord);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;
	std::optional<state_result> on_eof() override;

private:
	named_export_state* coord_;

	class visitor;
};

}  // namespace tscc::parse::state
