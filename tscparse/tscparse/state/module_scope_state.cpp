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

#include "module_scope_state.hpp"
#include <tsclex/tokens/import_token.hpp>
#include "import_state.hpp"
#include "state_result.hpp"

namespace tscc::parse {

namespace {

/**
 * \brief Visitor for module scope token processing
 *
 * Inherits from basic_state_visitor which provides the fallback
 * that throws declaration_or_statement_expected (TS1128).
 * Token-specific handlers are added as operator() overloads.
 */
class module_scope_visitor : public basic_state_visitor {
public:
	module_scope_visitor(module_scope_state* s,
						 const lex::source_location& loc,
						 const lex::token& token) noexcept
		: basic_state_visitor(s, loc), token_(token) {}

	state_result operator()(const lex::tokens::import_token&) const {
		return state_result::push<import_state>(token_);
	}

	using basic_state_visitor::operator();

	// TODO: Add handlers for tokens valid at module scope:
	// - export_token -> parse export declaration
	// - class_token -> push class_declaration_state
	// - function_token -> push function_declaration_state
	// - interface_token -> parse interface declaration
	// - enum_token -> parse enum declaration
	// - type_token -> parse type alias
	// - const_token, let_token, var_token -> parse variable declaration
	// - declare_token -> parse ambient declaration
	// - async_token, abstract_token -> push modifier state
	// - namespace_token, module_token -> parse namespace/module

private:
	const lex::token& token_;
};

}  // namespace

state_result module_scope_state::process(parser& /*p*/,
										 const lex::token& token) {
	return token.visit(module_scope_visitor{this, token.location(), token});
}

accept_result module_scope_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	return accept_result::stay();
}

}  // namespace tscc::parse
