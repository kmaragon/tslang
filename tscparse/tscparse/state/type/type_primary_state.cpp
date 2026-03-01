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

#include "type_primary_state.hpp"
#include <tsclex/tokens/any_token.hpp>
#include <tsclex/tokens/newline_token.hpp>
#include <tsclex/tokens/bigint_token.hpp>
#include <tsclex/tokens/boolean_token.hpp>
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/false_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/minus_token.hpp>
#include <tsclex/tokens/never_token.hpp>
#include <tsclex/tokens/null_token.hpp>
#include <tsclex/tokens/number_token.hpp>
#include <tsclex/tokens/object_token.hpp>
#include <tsclex/tokens/open_paren_token.hpp>
#include <tsclex/tokens/string_token.hpp>
#include <tsclex/tokens/symbol_token.hpp>
#include <tsclex/tokens/this_token.hpp>
#include <tsclex/tokens/true_token.hpp>
#include <tsclex/tokens/undefined_token.hpp>
#include <tsclex/tokens/unknown_token.hpp>
#include <tsclex/tokens/void_token.hpp>
#include "../../ast/type/keyword_type_node.hpp"
#include "../../ast/type/literal_type_node.hpp"
#include "../../ast/type/this_type_node.hpp"
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "../token_helpers.hpp"
#include "type_paren_state.hpp"
#include "type_reference_state.hpp"

using namespace tscc::parse::state;

namespace {

enum class primary_action {
	keyword_type,
	literal_type,
	negative_literal,
	this_type,
	type_reference,
	open_paren,
	error,
};

struct primary_classifier {
	primary_action operator()(const tscc::lex::tokens::string_token&) const { return primary_action::keyword_type; }
	primary_action operator()(const tscc::lex::tokens::number_token&) const { return primary_action::keyword_type; }
	primary_action operator()(const tscc::lex::tokens::boolean_token&) const { return primary_action::keyword_type; }
	primary_action operator()(const tscc::lex::tokens::void_token&) const { return primary_action::keyword_type; }
	primary_action operator()(const tscc::lex::tokens::never_token&) const { return primary_action::keyword_type; }
	primary_action operator()(const tscc::lex::tokens::unknown_token&) const { return primary_action::keyword_type; }
	primary_action operator()(const tscc::lex::tokens::any_token&) const { return primary_action::keyword_type; }
	primary_action operator()(const tscc::lex::tokens::object_token&) const { return primary_action::keyword_type; }
	primary_action operator()(const tscc::lex::tokens::symbol_token&) const { return primary_action::keyword_type; }
	primary_action operator()(const tscc::lex::tokens::bigint_token&) const { return primary_action::keyword_type; }
	primary_action operator()(const tscc::lex::tokens::undefined_token&) const { return primary_action::keyword_type; }
	primary_action operator()(const tscc::lex::tokens::null_token&) const { return primary_action::keyword_type; }

	primary_action operator()(const tscc::lex::tokens::true_token&) const { return primary_action::literal_type; }
	primary_action operator()(const tscc::lex::tokens::false_token&) const { return primary_action::literal_type; }
	primary_action operator()(const tscc::lex::tokens::constant_value_token&) const { return primary_action::literal_type; }

	primary_action operator()(const tscc::lex::tokens::minus_token&) const { return primary_action::negative_literal; }
	primary_action operator()(const tscc::lex::tokens::this_token&) const { return primary_action::this_type; }
	primary_action operator()(const tscc::lex::tokens::identifier_token&) const { return primary_action::type_reference; }
	primary_action operator()(const tscc::lex::tokens::open_paren_token&) const { return primary_action::open_paren; }

	primary_action operator()(const tscc::lex::tokens::basic_token&) const { return primary_action::error; }
};

}  // namespace

type_primary_state::type_primary_state(ast::type_context ctx) : ctx_(ctx) {}

state_result type_primary_state::process(parser& /*p*/,
										 const lex::token& token) {
	if (token.is<lex::tokens::newline_token>()) return state_result::stay();

	if (minus_) {
		if (!token.is<lex::tokens::constant_value_token>()) {
			throw expected_token(token.location(), "numeric literal",
								 token->to_string());
		}
		return state_result::complete(
			std::make_unique<ast::literal_type_node>(std::move(minus_), token));
	}

	auto action = token.visit(primary_classifier{});

	switch (action) {
	case primary_action::keyword_type:
		return state_result::complete(
			std::make_unique<ast::keyword_type_node>(token));

	case primary_action::literal_type:
		return state_result::complete(
			std::make_unique<ast::literal_type_node>(token));

	case primary_action::negative_literal:
		minus_ = token;
		return state_result::stay();

	case primary_action::this_type:
		return state_result::complete(
			std::make_unique<ast::this_type_node>(token));

	case primary_action::type_reference:
		return state_result::push<type_reference_state>(ctx_).reprocess();

	case primary_action::open_paren:
		return state_result::push<type_paren_state>(token, ctx_);

	case primary_action::error:
		throw expected_token(token.location(), "type", token->to_string());
	}

	// Unreachable, but silence compiler warnings.
	throw expected_token(token.location(), "type", token->to_string());
}

accept_result type_primary_state::accept_child(
	std::unique_ptr<ast::ast_node> child) {
	return accept_result::complete(std::move(child));
}
