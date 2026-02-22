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

#include "declare_module_header_state.hpp"
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/open_brace_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"

using namespace tscc;
using namespace tscc::parse;
using namespace tscc::parse::state;

namespace {

/**
 * \brief Validates that the token is a string literal for the module name
 *
 * Throws expected_token for any non-string token. Returns stay() for
 * string literals so the caller can store the token.
 */
class module_name_visitor : public basic_state_visitor {
public:
	module_name_visitor(parser_state* s,
						const lex::source_location& loc,
						const lex::token& token) noexcept
		: basic_state_visitor(s, loc), token_(token) {}

	state_result operator()(const lex::tokens::constant_value_token& cv) const {
		if (!cv.is_string()) {
			throw expected_token(location, "module name (string literal)",
								 token_->to_string());
		}
		return state_result::stay();
	}

	// TODO: identifier form (declare module Foo { ... }) should push
	// the same state as declare namespace for the identifier case.

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "module name (string literal)",
							 token_->to_string());
	}

private:
	const lex::token& token_;
};

}  // namespace

declare_module_header_state::declare_module_header_state(
	ast::declare_module_node* node)
	: node_(node) {}

state_result declare_module_header_state::process(parser& /*p*/,
												  const lex::token& token) {
	if (!has_name_) {
		has_name_ = true;
		token.visit(
			module_name_visitor{this, token.location(), token});
		node_->module_name_ = token;
		return state_result::stay();
	}

	if (token.is<lex::tokens::open_brace_token>()) {
		node_->open_brace_ = token;
		return state_result::complete(nullptr);
	}

	throw expected_token(token.location(), "{", token->to_string());
}

accept_result declare_module_header_state::accept_child(
	std::unique_ptr<ast::ast_node>) {
	throw std::logic_error(
		"declare_module_header_state does not push child states");
}
