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

#include "specifier_state.hpp"
#include <tsclex/tokens/as_token.hpp>
#include <tsclex/tokens/close_brace_token.hpp>
#include <tsclex/tokens/comma_token.hpp>
#include <tsclex/tokens/newline_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "../token_helpers.hpp"
#include "specifier_alias_state.hpp"
#include "specifier_head_state.hpp"
#include "specifier_list_state.hpp"

using namespace tscc::parse::state;

specifier_state::specifier_state(specifier_list_state* list) : list_(list) {}

state_result specifier_state::process(parser& /*p*/, const lex::token& token) {
	if (token.is<lex::tokens::newline_token>()) return state_result::stay();

	if (!head_done_) {
		return state_result::push<specifier_head_state>(this).reprocess();
	}

	if (token.is<lex::tokens::as_token>()) {
		return state_result::push<specifier_alias_state>(this);
	}
	if (token.is<lex::tokens::comma_token>() ||
		token.is<lex::tokens::close_brace_token>()) {
		return flush_and_complete().reprocess();
	}

	throw expected_token(token.location(), "'as', ',', or '}'",
						 token->to_string());
}

accept_result specifier_state::accept_child(std::unique_ptr<ast::ast_node>) {
	if (!head_done_) {
		head_done_ = true;
		return accept_result::stay();
	}
	detail::normalize_identifier(name_);
	detail::normalize_identifier(alias_);
	list_->store_specifier(std::move(name_), std::move(type_keyword_),
						   std::move(alias_));
	return accept_result::complete(nullptr);
}

state_result specifier_state::flush_and_complete() {
	detail::normalize_identifier(name_);
	list_->store_specifier(std::move(name_), std::move(type_keyword_),
						   std::move(alias_));
	return state_result::complete(nullptr);
}
