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

#include "export_star_state.hpp"
#include <tsclex/tokens/newline_token.hpp>
#include "../state_result.hpp"
#include "export_star_chain_states.hpp"

using namespace tscc::parse::state;

export_star_state::export_star_state(lex::token asterisk_token)
	: node_(std::make_unique<ast::export_star_node>(
		  std::move(asterisk_token))) {}

state_result export_star_state::process(parser& /*p*/,
										const lex::token& token) {
	if (token.is<lex::tokens::newline_token>()) return state_result::stay();

	started_ = true;
	return state_result::push<export_star_after_star_state>(this).reprocess();
}

accept_result export_star_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	return accept_result::complete(std::move(node_));
}

void export_star_state::set_as_name(lex::token tok) {
	node_->as_name_ = std::move(tok);
}

void export_star_state::set_module_specifier(lex::token tok) {
	node_->module_specifier_ = std::move(tok);
}

void export_star_state::set_semicolon(lex::token tok) {
	node_->semicolon_ = std::move(tok);
}
