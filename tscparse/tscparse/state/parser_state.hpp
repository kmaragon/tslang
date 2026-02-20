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
#include <optional>
#include <tsclex/token.hpp>
#include <tsclex/tokens/basic_token.hpp>

namespace tscc::parse {
class parser;
namespace ast {
class ast_node;
}
}  // namespace tscc::parse

namespace tscc::parse::state {

class state_result;
class accept_result;

/**
 * \brief Base class for parser state machine states
 *
 * Each state represents a point in the grammar where specific tokens
 * are valid. States use inheritance to share common behavior and
 * prevent invalid token combinations through type-safe transitions.
 */
class parser_state {
public:
	virtual ~parser_state() = default;

	/**
	 * \brief Process a token and return what to do next
	 *
	 * \param p The parser instance (for consuming tokens)
	 * \param token The current token to process
	 * \return A state_result indicating stay/push/complete
	 */
	virtual state_result process(parser& p, const lex::token& token) = 0;

	/**
	 * \brief Called when a child state completes
	 *
	 * The parent state should integrate the child's AST result
	 * and return what to do next.
	 *
	 * \param child The AST node produced by the child state
	 * \return An accept_result indicating stay (absorbed, ready for tokens)
	 *         or complete (parent is also done, here's combined AST)
	 */
	virtual accept_result accept_child(
		std::unique_ptr<ast::ast_node> child) = 0;

	/**
	 * \brief Called when EOF is reached while this state is active
	 *
	 * States that can validly complete at EOF (e.g. import without
	 * trailing semicolon) override this to return a complete result.
	 * Default returns nullopt, which causes unexpected_end_of_text.
	 */
	virtual std::optional<state_result> on_eof();
};

/**
 * \brief Base visitor for parser state process() implementations
 *
 * Provides a fallback operator() for basic_token that throws
 * declaration_or_statement_expected (TS1128). State-specific visitors
 * inherit from this and add handlers for tokens they accept.
 */
class basic_state_visitor {
protected:
	parser_state* state;
	const lex::source_location& location;

public:
	basic_state_visitor(parser_state* s,
						const lex::source_location& loc) noexcept
		: state(s), location(loc) {}

	[[noreturn]] state_result operator()(const lex::tokens::basic_token&) const;
};

}  // namespace tscc::parse::state
