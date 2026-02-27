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

#include "type_union_state.hpp"
#include <tsclex/tokens/bar_token.hpp>
#include "../../ast/type/union_type_node.hpp"
#include "../state_result.hpp"
#include "type_intersection_state.hpp"

using namespace tscc::parse::state;

type_union_state::type_union_state(ast::type_context ctx) : ctx_(ctx) {}

state_result type_union_state::process(parser& /*p*/,
									   const lex::token& token) {
	if (!init_done_) {
		init_done_ = true;
		if (token.is<lex::tokens::bar_token>()) {
			return state_result::push<type_intersection_state>(ctx_);
		}
		return state_result::push<type_intersection_state>(ctx_).reprocess();
	}

	if (token.is<lex::tokens::bar_token>()) {
		return state_result::push<type_intersection_state>(ctx_);
	}

	if (members_.size() == 1) {
		auto* raw = const_cast<ast::type_definition*>(members_[0].release());
		return state_result::complete(std::unique_ptr<ast::ast_node>(raw))
			.reprocess();
	}

	return state_result::complete(
		std::make_unique<ast::union_type_node>(std::move(members_)))
		.reprocess();
}

accept_result type_union_state::accept_child(
	std::unique_ptr<ast::ast_node> child) {
	members_.emplace_back(
		std::unique_ptr<const ast::type_definition>(
			static_cast<const ast::type_definition*>(child.release())));
	return accept_result::stay();
}

std::optional<state_result> type_union_state::on_eof() {
	if (members_.size() == 1) {
		auto* raw = const_cast<ast::type_definition*>(members_[0].release());
		return state_result::complete(std::unique_ptr<ast::ast_node>(raw));
	}
	if (members_.size() > 1) {
		return state_result::complete(
			std::make_unique<ast::union_type_node>(std::move(members_)));
	}
	return std::nullopt;
}
