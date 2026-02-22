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
#include "../parser_state.hpp"
#include "import_node_builder.hpp"

namespace tscc::parse::state {

/**
 * \brief First phase after the import keyword
 *
 * Entered via reprocess from import_state. Dispatches on the first
 * meaningful token: string literal, type, *, {, or identifier.
 * Pushes named_import_state as a sub-state when seeing {.
 */
class after_import_state : public parser_state {
public:
	explicit after_import_state(import_node_builder* builder,
								bool equals_only = false);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	import_node_builder* builder_;
	bool equals_only_;

	enum class mode { initial, awaiting_sub, post_sub };
	mode mode_ = mode::initial;

	class visitor;
	class equals_only_visitor;
};

/**
 * \brief Phase after consuming a `type` token that may be a modifier or binding
 *
 * Receives the pending type token. Disambiguates based on the next token:
 * { or * or identifier means type was a modifier, from or comma means it
 * was a binding name, = means it was the equals-import name.
 * Pushes named_import_state as a sub-state when seeing {.
 */
class after_type_state : public parser_state {
public:
	after_type_state(import_node_builder* builder, lex::token type_tok);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	import_node_builder* builder_;
	lex::token pending_type_;

	enum class mode { initial, awaiting_sub, post_sub };
	mode mode_ = mode::initial;

	void classify_type_as_modifier();
	void classify_type_as_binding();

	class visitor;
};

}  // namespace tscc::parse::state
