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
#include "../ast/namespace_node.hpp"
#include "parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Coordinator state for namespace/module declarations
 *
 * Handles all four identifier-name entry paths:
 * - `namespace Foo { ... }` / `module Foo { ... }` (may or may not be ambient)
 * - `declare namespace Foo { ... }` / `declare module Foo { ... }` (always
 *   ambient)
 *
 * Pushes namespace_header_state to consume the qualified name and opening
 * brace, then processes body tokens via module_scope_visitor with
 * module_like=false.
 */
class namespace_state : public parser_state {
public:
	/**
	 * \brief Construct for bare namespace/module (no declare prefix)
	 *
	 * \param keyword The namespace or module keyword token
	 * \param ambient Whether the enclosing context is ambient
	 */
	namespace_state(lex::token keyword, bool ambient);

	/**
	 * \brief Construct for declare namespace/module (always ambient)
	 *
	 * \param declare_keyword The declare keyword token
	 * \param keyword The namespace or module keyword token
	 */
	namespace_state(lex::token declare_keyword, lex::token keyword);

	state_result process(parser& p, const lex::token& token) override;

	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	std::unique_ptr<ast::namespace_node> node_;
	bool ambient_;
	bool header_done_ = false;
};

}  // namespace tscc::parse::state
