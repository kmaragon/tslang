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

#include "namespace_import_states.hpp"
#include <stdexcept>
#include <tsclex/tokens/as_token.hpp>
#include <tsclex/tokens/assert_token.hpp>
#include <tsclex/tokens/from_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/require_token.hpp>
#include <tsclex/tokens/type_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "from_clause_states.hpp"

namespace tscc::parse {

expect_as_state::expect_as_state(ast::import_node* node,
								 lex::token asterisk_tok)
	: node_(node), pending_asterisk_(std::move(asterisk_tok)) {}

class expect_as_state::visitor : public basic_state_visitor {
public:
	visitor(expect_as_state* s,
			const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::as_token&) const {
		return state_result::push<expect_namespace_name_state>(
			s_->node_, std::move(s_->pending_asterisk_), token_);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "'as'", token_->to_string());
	}

private:
	expect_as_state* s_;
	const lex::token& token_;
};

state_result expect_as_state::process(parser& /*p*/, const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result expect_as_state::accept_child(std::unique_ptr<ast::ast_node>) {
	return accept_result::complete(nullptr);
}

expect_namespace_name_state::expect_namespace_name_state(
	ast::import_node* node,
	lex::token asterisk_tok,
	lex::token as_tok)
	: node_(node),
	  pending_asterisk_(std::move(asterisk_tok)),
	  pending_as_(std::move(as_tok)) {}

class expect_namespace_name_state::visitor : public basic_state_visitor {
public:
	visitor(expect_namespace_name_state* s,
			const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::identifier_token&) const {
		return handle_identifier();
	}
	state_result operator()(const lex::tokens::type_token&) const {
		return handle_identifier();
	}
	state_result operator()(const lex::tokens::from_token&) const {
		return handle_identifier();
	}
	state_result operator()(const lex::tokens::as_token&) const {
		return handle_identifier();
	}
	state_result operator()(const lex::tokens::assert_token&) const {
		return handle_identifier();
	}
	state_result operator()(const lex::tokens::require_token&) const {
		return handle_identifier();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "identifier", token_->to_string());
	}

private:
	state_result handle_identifier() const {
		s_->node_->set_namespace_binding(std::move(s_->pending_asterisk_),
										 std::move(s_->pending_as_), token_);
		return state_result::push<expect_from_state>(s_->node_);
	}

	expect_namespace_name_state* s_;
	const lex::token& token_;
};

state_result expect_namespace_name_state::process(parser& /*p*/,
												  const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result expect_namespace_name_state::accept_child(
	std::unique_ptr<ast::ast_node>) {
	return accept_result::complete(nullptr);
}

}  // namespace tscc::parse
