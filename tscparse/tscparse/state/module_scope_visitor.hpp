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

#include <tsclex/tokens/declare_token.hpp>
#include <tsclex/tokens/export_token.hpp>
#include <tsclex/tokens/import_token.hpp>
#include <tsclex/tokens/module_token.hpp>
#include <tsclex/tokens/namespace_token.hpp>
#include <tsclex/tokens/type_token.hpp>
#include "../error/declare_in_ambient_context.hpp"
#include "declare_state.hpp"
#include "export_state.hpp"
#include "import_state.hpp"
#include "namespace_state.hpp"
#include "parser_state.hpp"
#include "state_result.hpp"
#include "type_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Visitor for declaration scope token processing
 *
 * Handles content tokens valid at declaration scopes: module (file) scope,
 * namespace scope, ambient module, and ambient namespace. These four scopes
 * share ~90% of their legal constructs, differing only along two axes:
 *
 * - \p ambient: when true, `declare` throws TS1038 and statements are illegal.
 *   Set by declare_module_state and future declare_namespace_state.
 * - \p module_like: when true, full ES imports are allowed. When false, only
 *   `import X = Y.Z` alias form is legal (namespace scopes).
 *
 * Framing tokens like `}` are handled by the owning state before delegating
 * to this visitor.
 *
 * Inherits from basic_state_visitor which provides the fallback
 * that throws declaration_or_statement_expected (TS1128).
 * Token-specific handlers are added as operator() overloads.
 */
class module_scope_visitor : public basic_state_visitor {
public:
	module_scope_visitor(parser_state* s,
						 const lex::source_location& loc,
						 const lex::token& token,
						 bool ambient = false,
						 bool module_like = true) noexcept
		: basic_state_visitor(s, loc),
		  token_(token),
		  ambient_(ambient),
		  module_like_(module_like) {}

	state_result operator()(const lex::tokens::import_token&) const {
		return state_result::push<import_state>(token_,
												/*equals_only=*/!module_like_);
	}

	state_result operator()(const lex::tokens::declare_token&) const {
		if (ambient_) {
			throw declare_in_ambient_context(location);
		}

		return state_result::push<declare_state>(token_);
	}

	state_result operator()(const lex::tokens::namespace_token&) const {
		return state_result::push<namespace_state>(token_, ambient_);
	}

	state_result operator()(const lex::tokens::module_token&) const {
		return state_result::push<namespace_state>(token_, ambient_);
	}

	state_result operator()(const lex::tokens::export_token&) const {
		return state_result::push<export_state>(token_);
	}

	state_result operator()(const lex::tokens::type_token&) const {
		return state_result::push<type_state>(token_);
	}

	using basic_state_visitor::operator();

	// TODO: Add handlers for tokens valid at declaration scopes:
	// - class_token -> push class_declaration_state
	// - function_token -> push function_declaration_state
	// - interface_token -> parse interface declaration
	// - enum_token -> parse enum declaration
	// - const_token, let_token, var_token -> parse variable declaration
	// - async_token, abstract_token -> push modifier state

private:
	const lex::token& token_;
	bool ambient_;
	bool module_like_;
};

}  // namespace tscc::parse::state
