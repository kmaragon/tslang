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

#include <optional>
#include "../parser_state.hpp"
#include "import_node_builder.hpp"

namespace tscc::parse::state {

/**
 * \brief Parser sub-state for the contents of a named import list
 *
 * Handles specifiers, aliases, commas, inline type modifiers, and the
 * close brace. Completes with nullptr since no separate AST node is
 * produced.
 */
class named_import_state : public parser_state {
public:
	explicit named_import_state(import_node_builder* builder);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	enum class phase {
		expect_specifier_or_close,
		after_maybe_type,
		after_name,
		after_as,
		after_alias,
	};

	import_node_builder* builder_;
	phase phase_ = phase::expect_specifier_or_close;

	std::optional<lex::token> pending_type_;
	std::optional<lex::token> pending_name_;

	void flush_specifier();
	void flush_specifier_with_alias(lex::token alias);

	class expect_specifier_or_close_visitor;
	class after_maybe_type_visitor;
	class after_name_visitor;
	class after_as_visitor;
	class after_alias_visitor;
};

}  // namespace tscc::parse::state
