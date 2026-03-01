/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2025. Keef Aragon
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
#include <optional>
#include <tsclex/lexer.hpp>
#include <tsclex/token.hpp>
#include <vector>
#include "ast/ast_node.hpp"
#include "ast/source_file_node.hpp"
#include "parser_observer.hpp"
#include "state/parser_state.hpp"
#include "state/state_result.hpp"
#include "trivia_index.hpp"

namespace tscc::parse {

/**
 * \brief Parser that eagerly produces a complete AST from a token stream
 *
 * Consumes all tokens from the lexer and returns a source_file_node
 * containing all top-level declarations.
 */
class parser {
public:
	/**
	 * \brief Set a global observer for parser state transitions
	 *
	 * The observer receives on_push() calls whenever any parser instance
	 * pushes a new state. Set to nullptr to disable observation.
	 * Not thread-safe — set once before parsing begins.
	 */
	static void set_observer(parser_observer* obs) noexcept;

	/**
	 * \brief Construct a parser
	 *
	 * \param lexer The lexer to consume tokens from
	 * \param trivia_idx Optional trivia index to populate during parsing (can
	 * be nullptr)
	 */
	parser(lex::lexer& lexer, trivia_index* trivia_idx = nullptr);

	// Disable copy / move
	parser(const parser&) = delete;
	parser(parser&&) = delete;

	/**
	 * \brief Parse the entire source file and return the root AST node
	 */
	std::unique_ptr<ast::source_file_node> parse();

private:
	static parser_observer* observer_;

	lex::lexer& lexer_;
	lex::lexer::iterator token_iter_;
	lex::lexer::iterator token_end_;
	trivia_index* trivia_index_;
	std::vector<lex::token> pending_trivia_;
	lex::source_location
		last_location_;	 // Track last valid location for EOF errors
	std::optional<lex::token> synthetic_newline_;
	std::vector<std::unique_ptr<state::parser_state>> state_stack_;

	// Internal: advance token iterator
	void advance_token();

	// Internal: get current token
	const lex::token& current_token() const;

	// Internal: check if at end of tokens
	bool at_token_end() const;

	// Internal: consume token and return it
	lex::token consume_token();

	// Skip trivia tokens and collect them
	void collect_trivia();

	// Flush collected trivia to the trivia index associated with a node
	void flush_trivia(ast::ast_node* node);

	// Handle a complete state_result: pop state, flush trivia, pass to parent.
	// triggering_token is the token that caused completion (nullptr for EOF).
	void handle_complete(state::state_result result,
						 const lex::token* triggering_token);

	// Expect a specific token type or throw
	template <typename TokenType>
	lex::token expect_token(std::string_view token_name);

	// Try to consume a modifier token (returns nullopt if not a modifier)
	std::optional<lex::token> try_consume_modifier();

	// Parse modifiers (any order)
	std::vector<lex::token> parse_modifiers();
};

}  // namespace tscc::parse
