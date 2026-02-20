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

#include "module_completion_states.hpp"
#include <stdexcept>
#include <tsclex/tokens/assert_token.hpp>
#include <tsclex/tokens/semicolon_token.hpp>
#include <tsclex/tokens/with_token.hpp>
#include "../state_result.hpp"
#include "import_attributes_state.hpp"

using namespace tscc::parse::state;

after_module_spec_state::after_module_spec_state(import_node_builder* builder)
	: builder_(builder) {}

class after_module_spec_state::initial_visitor : public basic_state_visitor {
public:
	initial_visitor(after_module_spec_state* s,
					const lex::source_location& loc,
					const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::semicolon_token&) const {
		return state_result::complete(nullptr);
	}

	state_result operator()(const lex::tokens::assert_token&) const {
		return push_attributes();
	}

	state_result operator()(const lex::tokens::with_token&) const {
		return push_attributes();
	}

	// ASI: any other token completes the import without consuming it
	state_result operator()(const lex::tokens::basic_token&) const {
		return state_result::complete(nullptr).reprocess();
	}

private:
	state_result push_attributes() const {
		s_->builder_->set_attributes_keyword(token_);
		s_->mode_ = mode::awaiting_sub;
		return state_result::push<import_attributes_state>(s_->builder_);
	}

	after_module_spec_state* s_;
	const lex::token& token_;
};

class after_module_spec_state::post_sub_visitor : public basic_state_visitor {
public:
	post_sub_visitor(after_module_spec_state* s,
					 const lex::source_location& loc,
					 const lex::token& /*token*/) noexcept
		: basic_state_visitor(s, loc) {}

	state_result operator()(const lex::tokens::semicolon_token&) const {
		return state_result::complete(nullptr);
	}

	// ASI: any other token completes after attributes
	state_result operator()(const lex::tokens::basic_token&) const {
		return state_result::complete(nullptr).reprocess();
	}
};

state_result after_module_spec_state::process(parser& /*p*/,
											  const lex::token& token) {
	if (mode_ == mode::post_sub)
		return token.visit(post_sub_visitor{this, token.location(), token});
	return token.visit(initial_visitor{this, token.location(), token});
}

accept_result after_module_spec_state::accept_child(
	std::unique_ptr<ast::ast_node>) {
	if (mode_ == mode::awaiting_sub) {
		mode_ = mode::post_sub;
		return accept_result::stay();
	}
	return accept_result::complete(nullptr);
}

std::optional<state_result> after_module_spec_state::on_eof() {
	return state_result::complete(nullptr);
}
