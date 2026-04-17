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

namespace tscc::parse::state {

class export_star_state;

/**
 * \brief Position after `export *` — expects `as` or `from`
 *
 * On `as`: pushes export_star_name_state.
 * On `from`: pushes export_star_module_spec_state.
 */
class export_star_after_star_state : public parser_state {
public:
	explicit export_star_after_star_state(export_star_state* coord);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	export_star_state* coord_;

	class visitor;
};

/**
 * \brief Position after `as` — expects the namespace identifier
 *
 * Stores the name on the coordinator and pushes export_star_from_state.
 */
class export_star_name_state : public parser_state {
public:
	explicit export_star_name_state(export_star_state* coord);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	export_star_state* coord_;

	class visitor;
};

/**
 * \brief Position expecting the `from` keyword
 *
 * Consumes `from` and pushes export_star_module_spec_state.
 */
class export_star_from_state : public parser_state {
public:
	explicit export_star_from_state(export_star_state* coord);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	export_star_state* coord_;

	class visitor;
};

/**
 * \brief Position after `from` — expects a string literal module specifier
 *
 * Stores the specifier on the coordinator and pushes export_star_semicolon_state.
 */
class export_star_module_spec_state : public parser_state {
public:
	explicit export_star_module_spec_state(export_star_state* coord);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	export_star_state* coord_;

	class visitor;
};

/**
 * \brief Terminator position — expects `;` or ASI
 *
 * On `;`: stores and completes. On other tokens: completes with
 * reprocess (ASI). On EOF: completes (ASI).
 */
class export_star_semicolon_state : public parser_state {
public:
	explicit export_star_semicolon_state(export_star_state* coord);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;
	std::optional<state_result> on_eof() override;

private:
	export_star_state* coord_;

	class visitor;
};

}  // namespace tscc::parse::state
