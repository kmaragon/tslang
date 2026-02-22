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

#include "../parser_state.hpp"
#include "import_node_builder.hpp"

namespace tscc::parse::state {

/**
 * \brief Expects `require` or an identifier after `=` in import-equals
 *
 * Dispatches to expect_require_paren_state or import_entity_state.
 */
class after_equals_state : public parser_state {
public:
	explicit after_equals_state(import_node_builder* builder,
								bool equals_only = false);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	import_node_builder* builder_;
	bool equals_only_;

	class visitor;
	class equals_only_visitor;
};

/**
 * \brief Expects the opening `(` after `require`
 */
class expect_require_paren_state : public parser_state {
public:
	explicit expect_require_paren_state(import_node_builder* builder);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	import_node_builder* builder_;

	class visitor;
};

/**
 * \brief Expects a string literal module specifier inside `require(`
 */
class after_require_open_state : public parser_state {
public:
	explicit after_require_open_state(import_node_builder* builder);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	import_node_builder* builder_;

	class visitor;
};

/**
 * \brief Expects `)` after the module specifier, then handles ;/ASI
 *
 * Uses a closed_ flag: before `)` only accepts close paren,
 * after `)` accepts semicolon or ASI.
 */
class after_require_module_state : public parser_state {
public:
	after_require_module_state();

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;
	std::optional<state_result> on_eof() override;

private:
	bool closed_ = false;

	class expect_close_visitor;
	class after_close_visitor;
};

/**
 * \brief Handles a dot-separated identifier chain: `Foo.Bar.Baz`
 *
 * Uses an expecting_id_ flag: alternates between expecting a dot
 * (or ;/ASI to complete) and expecting an identifier.
 */
class import_entity_state : public parser_state {
public:
	explicit import_entity_state(import_node_builder* builder);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;
	std::optional<state_result> on_eof() override;

private:
	import_node_builder* builder_;
	bool expecting_id_ = false;

	class after_id_visitor;
	class expect_id_visitor;
};

}  // namespace tscc::parse::state
