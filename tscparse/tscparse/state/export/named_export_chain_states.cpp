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

#include "named_export_chain_states.hpp"
#include <stdexcept>
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/semicolon_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "named_export_state.hpp"

using namespace tscc::parse::state;

class named_export_module_spec_state::visitor : public basic_state_visitor {
public:
	visitor(named_export_module_spec_state* s,
			const lex::source_location& loc, const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::constant_value_token&) const {
		s_->coord_->set_module_specifier(token_);
		return state_result::push<named_export_semicolon_state>(s_->coord_);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "module specifier",
							 token_->to_string());
	}

	using basic_state_visitor::operator();

private:
	named_export_module_spec_state* s_;
	const lex::token& token_;
};

named_export_module_spec_state::named_export_module_spec_state(
	named_export_state* coord)
	: coord_(coord) {}

state_result named_export_module_spec_state::process(parser& /*p*/,
													 const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result named_export_module_spec_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	return accept_result::complete(nullptr);
}

class named_export_semicolon_state::visitor : public basic_state_visitor {
public:
	visitor(named_export_semicolon_state* s, const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::semicolon_token&) const {
		s_->coord_->set_semicolon(token_);
		return state_result::complete(nullptr);
	}

	state_result operator()(const lex::tokens::basic_token&) const {
		return state_result::complete(nullptr).reprocess();
	}

	using basic_state_visitor::operator();

private:
	named_export_semicolon_state* s_;
	const lex::token& token_;
};

named_export_semicolon_state::named_export_semicolon_state(
	named_export_state* coord)
	: coord_(coord) {}

state_result named_export_semicolon_state::process(parser& /*p*/,
												   const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result named_export_semicolon_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	throw std::logic_error(
		"named_export_semicolon_state does not push children");
}

std::optional<state_result> named_export_semicolon_state::on_eof() {
	return state_result::complete(nullptr);
}
