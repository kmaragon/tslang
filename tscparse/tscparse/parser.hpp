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
#include "state/parser_state.hpp"
#include "state/state_result.hpp"
#include "trivia_index.hpp"

namespace tscc::parse {

/**
 * \brief Parser that produces lazy stream of top-level AST nodes
 *
 * The parser mirrors the lexer's iterator-based design, lazily parsing
 * top-level declarations as they are requested.
 */
class parser {
public:
	struct sentinel {};	 // End marker - must be declared first

	class iterator {
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = std::unique_ptr<ast::ast_node>;
		using pointer = value_type*;
		using reference = value_type&;
		using iterator_category = std::input_iterator_tag;

		// Move-only iterator
		iterator(const iterator&) = delete;
		iterator(iterator&&) noexcept = default;

		iterator& operator=(const iterator&) = delete;
		iterator& operator=(iterator&&) noexcept = default;

		reference operator*() { return current_node_; }
		pointer operator->() { return &current_node_; }

		iterator& operator++();

		// Comparison with other iterators
		bool operator==(const iterator& other) const;
		bool operator!=(const iterator& other) const;

		// Comparison with sentinel
		bool operator==(sentinel) const;
		bool operator!=(sentinel) const;

	private:
		friend class parser;

		explicit iterator(parser* p);

		void advance();

		parser* parser_;
		std::unique_ptr<ast::ast_node> current_node_;
	};

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
	 * \brief Get an iterator to the AST nodes
	 */
	iterator begin();

	/**
	 * \brief Get sentinel for end of AST nodes
	 */
	sentinel end();

private:
	lex::lexer::iterator token_iter_;
	lex::lexer::iterator token_end_;
	trivia_index* trivia_index_;
	std::vector<lex::token> pending_trivia_;
	lex::source_location
		last_location_;	 // Track last valid location for EOF errors
	std::vector<std::unique_ptr<parser_state>> state_stack_;

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

	// Attach collected trivia to a node
	void attach_trivia_to_node(
		ast::ast_node* node,
		trivia_ref::relationship rel = trivia_ref::relationship::leading);

	// Parse top-level element (import, class, function, etc.)
	std::unique_ptr<ast::ast_node> parse_top_level_element();

	// Handle a complete state_result: pop state, yield if top-level,
	// otherwise pass to parent. Returns node if yielded, nullptr if absorbed.
	std::unique_ptr<ast::ast_node> handle_complete(state_result result);

	// Expect a specific token type or throw
	template <typename TokenType>
	lex::token expect_token(std::string_view token_name);

	// Try to consume a modifier token (returns nullopt if not a modifier)
	std::optional<lex::token> try_consume_modifier();

	// Parse modifiers (any order)
	std::vector<lex::token> parse_modifiers();
};

}  // namespace tscc::parse