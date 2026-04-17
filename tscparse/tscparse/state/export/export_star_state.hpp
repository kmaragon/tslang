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
#include "../../ast/export_star_node.hpp"
#include "../parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Root state for `export * [as name] from "module" [;]`
 *
 * Entered after `export *` has been consumed. On the first token,
 * pushes export_star_after_star_state to begin the chain of
 * single-position sub-states. The chain terminates with
 * export_star_semicolon_state consuming the optional terminator,
 * and the coordinator emits the completed AST node.
 */
class export_star_state : public parser_state {
public:
	explicit export_star_state(lex::token asterisk_token);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

	/**
	 * \brief Set the namespace alias after `as`
	 *
	 * Called by export_star_name_state.
	 */
	void set_as_name(lex::token tok);

	/**
	 * \brief Set the module specifier string literal
	 *
	 * Called by export_star_module_spec_state.
	 */
	void set_module_specifier(lex::token tok);

	/**
	 * \brief Set the trailing semicolon token
	 *
	 * Called by export_star_semicolon_state.
	 */
	void set_semicolon(lex::token tok);

private:
	std::unique_ptr<ast::export_star_node> node_;
	bool started_ = false;
};

}  // namespace tscc::parse::state
