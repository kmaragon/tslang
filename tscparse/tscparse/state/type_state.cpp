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

#include "type_state.hpp"
#include <tsclex/tokens/close_brace_token.hpp>
#include <tsclex/tokens/newline_token.hpp>
#include <tsclex/tokens/semicolon_token.hpp>
#include "../error/expected_token.hpp"
#include "state_result.hpp"
#include "type/type_expression_state.hpp"
#include "type/type_header_state.hpp"

using namespace tscc::parse::state;

class type_state::post_type_visitor : public basic_state_visitor {
public:
	post_type_visitor(type_state* s,
					  const lex::source_location& loc,
					  const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::semicolon_token&) const {
		s_->node_->semicolon_ = token_;
		return state_result::complete(std::move(s_->node_));
	}

	state_result operator()(const lex::tokens::newline_token&) const {
		return state_result::complete(std::move(s_->node_)).reprocess();
	}

	state_result operator()(const lex::tokens::close_brace_token&) const {
		return state_result::complete(std::move(s_->node_)).reprocess();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "';'", token_->to_string());
	}

	using basic_state_visitor::operator();

private:
	type_state* s_;
	const lex::token& token_;
};

type_state::type_state(lex::token type_keyword)
	: node_(std::make_unique<ast::type_node>(std::move(type_keyword))) {}

type_state::type_state(lex::token declare_keyword, lex::token type_keyword)
	: node_(std::make_unique<ast::type_node>(std::move(type_keyword))) {
	node_->declare_keyword_ = std::move(declare_keyword);
}

state_result type_state::process(parser& /*p*/, const lex::token& token) {
	if (!header_done_) {
		header_done_ = true;
		return state_result::push<type_header_state>(node_.get()).reprocess();
	}

	if (!type_done_) {
		type_done_ = true;
		return state_result::push<type_expression_state>().reprocess();
	}

	return token.visit(post_type_visitor{this, token.location(), token});
}

accept_result type_state::accept_child(std::unique_ptr<ast::ast_node> child) {
	if (!type_done_) {
		// Header completed (returns nullptr).
		return accept_result::stay();
	}

	// RHS type expression completed. Set parent before const-casting.
	child = node_->adopt_child(std::move(child));
	node_->type_ = std::unique_ptr<const ast::type_definition>(
		static_cast<const ast::type_definition*>(child.release()));
	return accept_result::stay();
}

std::optional<state_result> type_state::on_eof() {
	if (type_done_ && node_->type_) {
		return state_result::complete(std::move(node_));
	}
	return std::nullopt;
}
