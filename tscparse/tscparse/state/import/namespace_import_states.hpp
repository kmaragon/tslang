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
#include "../../ast/import_node.hpp"
#include "../parser_state.hpp"

namespace tscc::parse {

/**
 * \brief Expects the `as` keyword after `*` in a namespace import
 *
 * Receives the pending asterisk token. On seeing `as`, pushes
 * expect_namespace_name_state with both pending tokens.
 */
class expect_as_state : public parser_state {
public:
	expect_as_state(ast::import_node* node, lex::token asterisk_tok);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	ast::import_node* node_;
	lex::token pending_asterisk_;

	class visitor;
};

/**
 * \brief Expects an identifier for the namespace binding after `* as`
 *
 * Receives pending asterisk and as tokens. On seeing an identifier,
 * commits the namespace binding and pushes expect_from_state.
 */
class expect_namespace_name_state : public parser_state {
public:
	expect_namespace_name_state(ast::import_node* node,
								lex::token asterisk_tok,
								lex::token as_tok);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	ast::import_node* node_;
	lex::token pending_asterisk_;
	lex::token pending_as_;

	class visitor;
};

}  // namespace tscc::parse
