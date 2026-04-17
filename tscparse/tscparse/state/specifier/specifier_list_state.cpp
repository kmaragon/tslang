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

#include "specifier_list_state.hpp"
#include <tsclex/tokens/close_brace_token.hpp>
#include <tsclex/tokens/comma_token.hpp>
#include <tsclex/tokens/newline_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "specifier_state.hpp"

using namespace tscc::parse::state;

state_result specifier_list_state::process(parser& /*p*/,
										   const lex::token& token) {
	if (token.is<lex::tokens::newline_token>()) return state_result::stay();

	switch (phase_) {
		case phase::expect_specifier_or_close:
			if (token.is<lex::tokens::close_brace_token>()) {
				return on_list_complete();
			}
			return state_result::push<specifier_state>(this).reprocess();

		case phase::expect_comma_or_close:
			if (token.is<lex::tokens::comma_token>()) {
				phase_ = phase::expect_specifier_or_close;
				return state_result::stay();
			}
			if (token.is<lex::tokens::close_brace_token>()) {
				return on_list_complete();
			}
			throw expected_token(token.location(), "',' or '}'",
								 token->to_string());
	}

	throw expected_token(token.location(), "',' or '}'", token->to_string());
}

accept_result specifier_list_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	phase_ = phase::expect_comma_or_close;
	return accept_result::stay();
}
