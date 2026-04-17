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
#include "../../ast/named_export_node.hpp"
#include "../parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Root state for `export { ... } [from "module"] [;]`
 *
 * On the first token, pushes a specifier_list sub-state to parse the
 * braced list. Once the list completes, handles the post-list grammar:
 * `;` / ASI terminate directly; `from` pushes a chain of single-position
 * sub-states (module_spec → semicolon). The coordinator emits the
 * completed AST node via accept_child or on_eof.
 */
class named_export_state : public parser_state {
public:
	named_export_state();

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;
	std::optional<state_result> on_eof() override;

	/**
	 * \brief Append a specifier to the underlying node
	 *
	 * Called by the child specifier_list sub-state during parsing.
	 */
	void add_specifier(lex::token name, lex::token type_keyword,
					   lex::token alias);

	/**
	 * \brief Set the module specifier for a re-export
	 *
	 * Called by named_export_module_spec_state.
	 */
	void set_module_specifier(lex::token tok);

	/**
	 * \brief Set the trailing semicolon token
	 *
	 * Called directly by the coordinator and by named_export_semicolon_state.
	 */
	void set_semicolon(lex::token tok);

private:
	std::unique_ptr<ast::named_export_node> node_;
	bool list_done_ = false;
};

}  // namespace tscc::parse::state
