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

#include "initial_import_states.hpp"
#include <stdexcept>
#include <tsclex/tokens/as_token.hpp>
#include <tsclex/tokens/assert_token.hpp>
#include <tsclex/tokens/asterisk_token.hpp>
#include <tsclex/tokens/comma_token.hpp>
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/eq_token.hpp>
#include <tsclex/tokens/from_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/open_brace_token.hpp>
#include <tsclex/tokens/require_token.hpp>
#include <tsclex/tokens/type_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "binding_import_states.hpp"
#include "equals_import_states.hpp"
#include "from_clause_states.hpp"
#include "module_completion_states.hpp"
#include "named_import_state.hpp"
#include "namespace_import_states.hpp"

using namespace tscc::parse::state;

after_import_state::after_import_state(import_node_builder* builder,
									   bool equals_only)
	: builder_(builder), equals_only_(equals_only) {}

class after_import_state::visitor : public basic_state_visitor {
public:
	visitor(after_import_state* s,
			const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::constant_value_token&) const {
		s_->builder_->set_module_specifier(token_);
		return state_result::push<after_module_spec_state>(s_->builder_);
	}

	state_result operator()(const lex::tokens::type_token&) const {
		return state_result::push<after_type_state>(s_->builder_, token_);
	}

	state_result operator()(const lex::tokens::asterisk_token&) const {
		return state_result::push<expect_as_state>(s_->builder_);
	}

	state_result operator()(const lex::tokens::open_brace_token&) const {
		s_->builder_->init_named_imports();
		s_->mode_ = mode::awaiting_sub;
		return state_result::push<named_import_state>(s_->builder_);
	}

	state_result operator()(const lex::tokens::identifier_token&) const {
		return handle_identifier();
	}
	state_result operator()(const lex::tokens::from_token&) const {
		return handle_identifier();
	}
	state_result operator()(const lex::tokens::as_token&) const {
		return handle_identifier();
	}
	state_result operator()(const lex::tokens::assert_token&) const {
		return handle_identifier();
	}
	state_result operator()(const lex::tokens::require_token&) const {
		return handle_identifier();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(
			location, "module specifier, identifier, '*', '{', or 'type'",
			token_->to_string());
	}

private:
	state_result handle_identifier() const {
		return state_result::push<after_default_state>(s_->builder_, token_);
	}

	after_import_state* s_;
	const lex::token& token_;
};

/// Restricted visitor for namespace scope: only accepts identifiers
/// (the binding name for `import name = QualifiedName`).
/// Contextual keywords like `type` are treated as plain names here.
class after_import_state::equals_only_visitor : public basic_state_visitor {
public:
	equals_only_visitor(after_import_state* s,
						const lex::source_location& loc,
						const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::identifier_token&) const {
		return handle_name();
	}
	state_result operator()(const lex::tokens::type_token&) const {
		return handle_name();
	}
	state_result operator()(const lex::tokens::from_token&) const {
		return handle_name();
	}
	state_result operator()(const lex::tokens::as_token&) const {
		return handle_name();
	}
	state_result operator()(const lex::tokens::assert_token&) const {
		return handle_name();
	}
	state_result operator()(const lex::tokens::require_token&) const {
		return handle_name();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "identifier", token_->to_string());
	}

private:
	state_result handle_name() const {
		return state_result::push<after_default_state>(s_->builder_, token_,
													   true);
	}

	after_import_state* s_;
	const lex::token& token_;
};

state_result after_import_state::process(parser& /*p*/,
										 const lex::token& token) {
	if (equals_only_)
		return token.visit(equals_only_visitor{this, token.location(), token});
	if (mode_ == mode::post_sub)
		return state_result::push<expect_from_state>(builder_).reprocess();
	return token.visit(visitor{this, token.location(), token});
}

accept_result after_import_state::accept_child(std::unique_ptr<ast::ast_node>) {
	if (mode_ == mode::awaiting_sub) {
		mode_ = mode::post_sub;
		return accept_result::stay();
	}
	return accept_result::complete(nullptr);
}

after_type_state::after_type_state(import_node_builder* builder,
								   lex::token type_tok)
	: builder_(builder), pending_type_(std::move(type_tok)) {}

class after_type_state::visitor : public basic_state_visitor {
public:
	visitor(after_type_state* s,
			const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::open_brace_token&) const {
		s_->classify_type_as_modifier();
		s_->builder_->init_named_imports();
		s_->mode_ = mode::awaiting_sub;
		return state_result::push<named_import_state>(s_->builder_);
	}

	state_result operator()(const lex::tokens::asterisk_token&) const {
		s_->classify_type_as_modifier();
		return state_result::push<expect_as_state>(s_->builder_);
	}

	state_result operator()(const lex::tokens::from_token&) const {
		s_->classify_type_as_binding();
		return state_result::push<after_from_state>(s_->builder_);
	}

	state_result operator()(const lex::tokens::comma_token&) const {
		s_->classify_type_as_binding();
		return state_result::push<after_comma_state>(s_->builder_);
	}

	state_result operator()(const lex::tokens::eq_token&) const {
		s_->builder_->set_equals_name(std::move(s_->pending_type_));
		return state_result::push<after_equals_state>(s_->builder_);
	}

	state_result operator()(const lex::tokens::identifier_token&) const {
		return handle_identifier();
	}
	state_result operator()(const lex::tokens::type_token&) const {
		return handle_identifier();
	}
	state_result operator()(const lex::tokens::as_token&) const {
		return handle_identifier();
	}
	state_result operator()(const lex::tokens::assert_token&) const {
		return handle_identifier();
	}
	state_result operator()(const lex::tokens::require_token&) const {
		return handle_identifier();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "identifier, '*', '{', or 'from'",
							 token_->to_string());
	}

private:
	state_result handle_identifier() const {
		s_->classify_type_as_modifier();
		return state_result::push<after_default_state>(s_->builder_, token_);
	}

	after_type_state* s_;
	const lex::token& token_;
};

state_result after_type_state::process(parser& /*p*/, const lex::token& token) {
	if (mode_ == mode::post_sub)
		return state_result::push<expect_from_state>(builder_).reprocess();
	return token.visit(visitor{this, token.location(), token});
}

accept_result after_type_state::accept_child(std::unique_ptr<ast::ast_node>) {
	if (mode_ == mode::awaiting_sub) {
		mode_ = mode::post_sub;
		return accept_result::stay();
	}
	return accept_result::complete(nullptr);
}

void after_type_state::classify_type_as_modifier() {
	builder_->set_type_keyword(std::move(pending_type_));
}

void after_type_state::classify_type_as_binding() {
	builder_->set_default_binding(std::move(pending_type_));
}
