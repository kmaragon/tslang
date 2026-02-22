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
 * \brief Phase after consuming a default binding identifier
 *
 * The binding token is pending (not yet committed to the node).
 * Expects from, comma, or = to determine the import form.
 */
class after_default_state : public parser_state {
public:
	after_default_state(import_node_builder* builder,
						lex::token binding_tok,
						bool equals_only = false);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	import_node_builder* builder_;
	lex::token pending_binding_;
	bool equals_only_;

	class visitor;
	class equals_only_visitor;
};

/**
 * \brief Phase after consuming a comma following a default binding
 *
 * Expects * (namespace import) or { (named imports).
 * Pushes named_import_state as a sub-state when seeing {.
 */
class after_comma_state : public parser_state {
public:
	explicit after_comma_state(import_node_builder* builder);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	import_node_builder* builder_;

	enum class mode { initial, awaiting_sub, post_sub };
	mode mode_ = mode::initial;

	class visitor;
};

}  // namespace tscc::parse::state
