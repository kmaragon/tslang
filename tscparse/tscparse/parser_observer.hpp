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

namespace tscc::lex {
class token;
}

namespace tscc::parse {

namespace state {
class parser_state;
}

namespace ast {
class ast_node;
}

/**
 * \brief Abstract observer for parser state transitions
 *
 * Implementations receive notifications when the parser pushes a new
 * state onto the stack or when a state completes and emits an AST node.
 * This allows external tools (e.g. transition loggers) to discover the
 * state machine graph without modifying parser internals.
 */
class parser_observer {
public:
	virtual ~parser_observer() = default;

	/**
	 * \brief Called when the parser pushes a child state
	 *
	 * \param source The state that was active before the push
	 * \param token The token that triggered the transition
	 * \param target The newly pushed child state
	 */
	virtual void on_push(const state::parser_state& source,
						 const lex::token& token,
						 const state::parser_state& target) = 0;

	/**
	 * \brief Called when a state completes and produces an AST node
	 *
	 * \param state The state that completed
	 * \param token The token that triggered completion, or nullptr for EOF
	 * \param node The AST node produced by the state, or nullptr for
	 *             header/builder states that populate the parent directly
	 */
	virtual void on_complete(const state::parser_state& state,
							 const lex::token* token,
							 const ast::ast_node* node) = 0;

	/**
	 * \brief Called when a parent state cascades after accept_child
	 *
	 * The parent did not produce a new node — it forwarded its child's
	 * node and completed itself.
	 *
	 * \param state The parent state that cascaded
	 * \param token The token that triggered the original completion
	 * \param node The forwarded AST node, or nullptr
	 */
	virtual void on_cascade(const state::parser_state& state,
							 const lex::token* token,
							 const ast::ast_node* node) = 0;
};

}  // namespace tscc::parse
