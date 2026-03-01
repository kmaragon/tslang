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

#include "namespace_header_state.hpp"
#include <tsclex/tokens/dot_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/newline_token.hpp>
#include <tsclex/tokens/open_brace_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"

using namespace tscc;
using namespace tscc::parse::state;

namespace_header_state::namespace_header_state(ast::namespace_node* node)
	: node_(node) {}

state_result namespace_header_state::process(parser& /*p*/,
											 const lex::token& token) {
	if (token.is<lex::tokens::newline_token>())
		return state_result::stay();

	if (expect_segment_) {
		if (!token.is<lex::tokens::identifier_token>()) {
			throw expected_token(token.location(), "identifier",
								 token->to_string());
		}
		node_->name_.segments_.emplace_back(token);
		expect_segment_ = false;
		return state_result::stay();
	}

	if (token.is<lex::tokens::dot_token>()) {
		expect_segment_ = true;
		return state_result::stay();
	}

	if (token.is<lex::tokens::open_brace_token>()) {
		node_->open_brace_ = token;
		return state_result::complete(nullptr);
	}

	throw expected_token(token.location(), "'.' or '{'", token->to_string());
}

accept_result namespace_header_state::accept_child(
	std::unique_ptr<ast::ast_node>) {
	throw std::logic_error("namespace_header_state does not push child states");
}
