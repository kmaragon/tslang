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

#include "export_state.hpp"
#include <tsclex/tokens/asterisk_token.hpp>
#include <tsclex/tokens/declare_token.hpp>
#include <tsclex/tokens/default_token.hpp>
#include <tsclex/tokens/eq_token.hpp>
#include <tsclex/tokens/import_token.hpp>
#include <tsclex/tokens/module_token.hpp>
#include <tsclex/tokens/namespace_token.hpp>
#include <tsclex/tokens/open_brace_token.hpp>
#include <tsclex/tokens/type_token.hpp>
#include "../ast/exportable_node.hpp"
#include "../error/declaration_or_statement_expected.hpp"
#include "../error/export_assignment_not_at_top_level.hpp"
#include "../error/export_default_not_at_top_level.hpp"
#include "declare_state.hpp"
#include "export/export_assignment_state.hpp"
#include "export/export_star_state.hpp"
#include "export/named_export_state.hpp"
#include "import_state.hpp"
#include "namespace_state.hpp"
#include "state_result.hpp"
#include "type_state.hpp"

using namespace tscc;
using namespace tscc::parse;
using namespace tscc::parse::state;

namespace {

/**
 * \brief Visitor for routing the token after `export`
 *
 * Inherits from basic_state_visitor which provides the fallback
 * that throws declaration_or_statement_expected (TS1128).
 */
class export_visitor : public basic_state_visitor {
public:
	export_visitor(parser_state* s,
				   const lex::source_location& loc,
				   const lex::token& token,
				   bool module_like) noexcept
		: basic_state_visitor(s, loc),
		  token_(token),
		  module_like_(module_like) {}

	state_result operator()(const lex::tokens::type_token&) const {
		return state_result::push<type_state>(token_);
	}

	state_result operator()(const lex::tokens::namespace_token&) const {
		return state_result::push<namespace_state>(token_, false);
	}

	state_result operator()(const lex::tokens::module_token&) const {
		return state_result::push<namespace_state>(token_, false);
	}

	state_result operator()(const lex::tokens::import_token&) const {
		return state_result::push<import_state>(token_, /*equals_only=*/true);
	}

	state_result operator()(const lex::tokens::declare_token&) const {
		return state_result::push<declare_state>(token_);
	}

	state_result operator()(const lex::tokens::default_token&) const {
		if (!module_like_)
			throw export_default_not_at_top_level(location);
		// TODO: push export_default_state
		throw declaration_or_statement_expected(location);
	}

	state_result operator()(const lex::tokens::eq_token&) const {
		if (!module_like_)
			throw export_assignment_not_at_top_level(location);
		return state_result::push<export_assignment_state>(token_);
	}

	state_result operator()(const lex::tokens::asterisk_token&) const {
		return state_result::push<export_star_state>(token_);
	}

	state_result operator()(const lex::tokens::open_brace_token&) const {
		return state_result::push<named_export_state>();
	}

	using basic_state_visitor::operator();

	// TODO: Add handlers for tokens valid after `export`:
	// - class_token -> push class_declaration_state
	// - function_token -> push function_declaration_state
	// - interface_token -> push interface_declaration_state
	// - enum_token -> push enum_declaration_state
	// - const_token, let_token, var_token -> push variable_declaration_state
	// - async_token -> push async_function_declaration_state
	// - abstract_token -> push abstract_class_declaration_state

private:
	const lex::token& token_;
	bool module_like_;
};

}  // namespace

export_state::export_state(lex::token export_keyword, bool module_like)
	: export_keyword_(std::move(export_keyword)), module_like_(module_like) {}

state_result export_state::process(parser& /*p*/, const lex::token& token) {
	return token.visit(
		export_visitor{this, token.location(), token, module_like_});
}

accept_result export_state::accept_child(std::unique_ptr<ast::ast_node> child) {
	if (ast::ast_node::is_exportable(child->node_kind())) {
		static_cast<ast::exportable_node*>(child.get())
			->set_export_keyword(std::move(export_keyword_));
}
	return accept_result::complete(std::move(child));
}
