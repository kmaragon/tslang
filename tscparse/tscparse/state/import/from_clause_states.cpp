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

#include "from_clause_states.hpp"
#include <stdexcept>
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/from_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "module_completion_states.hpp"

namespace tscc::parse {

expect_from_state::expect_from_state(ast::import_node* node) : node_(node) {}

class expect_from_state::visitor : public basic_state_visitor {
public:
	visitor(expect_from_state* s,
			const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::from_token&) const {
		s_->node_->set_from_keyword(token_);
		return state_result::push<after_from_state>(s_->node_);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "'from'", token_->to_string());
	}

private:
	expect_from_state* s_;
	const lex::token& token_;
};

state_result expect_from_state::process(parser& /*p*/,
										const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result expect_from_state::accept_child(std::unique_ptr<ast::ast_node>) {
	return accept_result::complete(nullptr);
}

after_from_state::after_from_state(ast::import_node* node) : node_(node) {}

class after_from_state::visitor : public basic_state_visitor {
public:
	visitor(after_from_state* s,
			const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::constant_value_token&) const {
		s_->node_->set_module_specifier(token_);
		return state_result::push<after_module_spec_state>(s_->node_);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "module specifier", token_->to_string());
	}

private:
	after_from_state* s_;
	const lex::token& token_;
};

state_result after_from_state::process(parser& /*p*/, const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result after_from_state::accept_child(std::unique_ptr<ast::ast_node>) {
	return accept_result::complete(nullptr);
}

}  // namespace tscc::parse
