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

#include "declare_state.hpp"
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/module_token.hpp>
#include <tsclex/tokens/namespace_token.hpp>
#include "declare/declare_module_state.hpp"
#include "namespace_state.hpp"
#include "state_result.hpp"

using namespace tscc;
using namespace tscc::parse;
using namespace tscc::parse::state;

namespace {

/**
 * \brief Disambiguates `declare module "string"` from `declare module
 * Identifier`
 *
 * Receives the name token (the one after `declare module`), inspects it, and
 * pushes the appropriate state with reprocess so the child's header also sees
 * the name token.
 */
class declare_module_dispatch_state : public parser_state {
public:
	declare_module_dispatch_state(lex::token declare_keyword,
								  lex::token module_keyword)
		: declare_keyword_(std::move(declare_keyword)),
		  module_keyword_(std::move(module_keyword)) {}

	state_result process(parser& /*p*/, const lex::token& token) override {
		if (token.is<lex::tokens::constant_value_token>()) {
			return state_result::push<declare_module_state>(
					   std::move(declare_keyword_), std::move(module_keyword_))
				.reprocess();
		}
		return state_result::push<namespace_state>(
				   std::move(declare_keyword_), std::move(module_keyword_))
			.reprocess();
	}

	accept_result accept_child(
		std::unique_ptr<ast::ast_node> child) override {
		return accept_result::complete(std::move(child));
	}

private:
	lex::token declare_keyword_;
	lex::token module_keyword_;
};

/**
 * \brief Visitor for routing the token after `declare`
 *
 * Inherits from basic_state_visitor which provides the fallback
 * that throws declaration_or_statement_expected (TS1128).
 */
class declare_visitor : public basic_state_visitor {
public:
	declare_visitor(parser_state* s,
					const lex::source_location& loc,
					const lex::token& declare_keyword,
					const lex::token& token) noexcept
		: basic_state_visitor(s, loc),
		  declare_keyword_(declare_keyword),
		  token_(token) {}

	state_result operator()(const lex::tokens::module_token&) const {
		return state_result::push<declare_module_dispatch_state>(
			declare_keyword_, token_);
	}

	state_result operator()(const lex::tokens::namespace_token&) const {
		return state_result::push<namespace_state>(declare_keyword_, token_);
	}

	using basic_state_visitor::operator();

	// TODO: Add handlers for tokens valid after `declare`:
	// - var_token -> push declare_var_state
	// - let_token -> push declare_let_state
	// - const_token -> push declare_const_state
	// - function_token -> push declare_function_state
	// - class_token -> push declare_class_state
	// - enum_token -> push declare_enum_state
	// - global_token -> push declare_global_state

private:
	const lex::token& declare_keyword_;
	const lex::token& token_;
};

}  // namespace

declare_state::declare_state(lex::token declare_keyword)
	: declare_keyword_(std::move(declare_keyword)) {}

state_result declare_state::process(parser& /*p*/, const lex::token& token) {
	return token.visit(
		declare_visitor{this, token.location(), declare_keyword_, token});
}

accept_result declare_state::accept_child(
	std::unique_ptr<ast::ast_node> child) {
	return accept_result::complete(std::move(child));
}
