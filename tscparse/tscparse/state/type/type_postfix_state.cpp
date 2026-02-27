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

#include "type_postfix_state.hpp"
#include <tsclex/tokens/close_bracket_token.hpp>
#include <tsclex/tokens/open_bracket_token.hpp>
#include "../../ast/type/array_type_node.hpp"
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "type_primary_state.hpp"

using namespace tscc::parse::state;

type_postfix_state::type_postfix_state(ast::type_context ctx) : ctx_(ctx) {}

state_result type_postfix_state::process(parser& /*p*/,
										 const lex::token& token) {
	if (!init_done_) {
		init_done_ = true;
		return state_result::push<type_primary_state>(ctx_).reprocess();
	}

	// After seeing `[`, expect `]` for array type.
	if (open_bracket_) {
		if (token.is<lex::tokens::close_bracket_token>()) {
			auto element = std::move(base_);
			base_ = std::make_unique<ast::array_type_node>(
				std::move(element), std::move(open_bracket_), token);
			open_bracket_ = lex::token{};
			// Stay to check for more postfix operators (`[][]`).
			return state_result::stay();
		}
		// Phase 2: push type_expression_state for indexed access type.
		throw expected_token(token.location(), "']'", token->to_string());
	}

	if (token.is<lex::tokens::open_bracket_token>()) {
		open_bracket_ = token;
		return state_result::stay();
	}

	auto* raw = const_cast<ast::type_definition*>(base_.release());
	return state_result::complete(std::unique_ptr<ast::ast_node>(raw))
		.reprocess();
}

accept_result type_postfix_state::accept_child(
	std::unique_ptr<ast::ast_node> child) {
	base_ = std::unique_ptr<const ast::type_definition>(
		static_cast<const ast::type_definition*>(child.release()));
	return accept_result::stay();
}

std::optional<state_result> type_postfix_state::on_eof() {
	if (base_ && !open_bracket_) {
		auto* raw = const_cast<ast::type_definition*>(base_.release());
		return state_result::complete(std::unique_ptr<ast::ast_node>(raw));
	}
	return std::nullopt;
}
