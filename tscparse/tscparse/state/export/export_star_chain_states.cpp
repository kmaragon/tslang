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

#include "export_star_chain_states.hpp"
#include <stdexcept>
#include <tsclex/tokens/as_token.hpp>
#include <tsclex/tokens/assert_token.hpp>
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/from_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/require_token.hpp>
#include <tsclex/tokens/semicolon_token.hpp>
#include <tsclex/tokens/type_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "export_star_state.hpp"

using namespace tscc::parse::state;

class export_star_after_star_state::visitor : public basic_state_visitor {
public:
	visitor(export_star_after_star_state* s, const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::as_token&) const {
		return state_result::push<export_star_name_state>(s_->coord_);
	}

	state_result operator()(const lex::tokens::from_token&) const {
		return state_result::push<export_star_module_spec_state>(s_->coord_);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "'from'", token_->to_string());
	}

	using basic_state_visitor::operator();

private:
	export_star_after_star_state* s_;
	const lex::token& token_;
};

export_star_after_star_state::export_star_after_star_state(
	export_star_state* coord)
	: coord_(coord) {}

state_result export_star_after_star_state::process(parser& /*p*/,
												   const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result export_star_after_star_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	return accept_result::complete(nullptr);
}

class export_star_name_state::visitor : public basic_state_visitor {
public:
	visitor(export_star_name_state* s, const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::identifier_token&) const {
		return adopt_name();
	}
	state_result operator()(const lex::tokens::from_token&) const {
		return adopt_name();
	}
	state_result operator()(const lex::tokens::as_token&) const {
		return adopt_name();
	}
	state_result operator()(const lex::tokens::type_token&) const {
		return adopt_name();
	}
	state_result operator()(const lex::tokens::assert_token&) const {
		return adopt_name();
	}
	state_result operator()(const lex::tokens::require_token&) const {
		return adopt_name();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "identifier", token_->to_string());
	}

	using basic_state_visitor::operator();

private:
	state_result adopt_name() const {
		s_->coord_->set_as_name(token_);
		return state_result::push<export_star_from_state>(s_->coord_);
	}

	export_star_name_state* s_;
	const lex::token& token_;
};

export_star_name_state::export_star_name_state(export_star_state* coord)
	: coord_(coord) {}

state_result export_star_name_state::process(parser& /*p*/,
											 const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result export_star_name_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	return accept_result::complete(nullptr);
}

class export_star_from_state::visitor : public basic_state_visitor {
public:
	visitor(export_star_from_state* s, const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::from_token&) const {
		return state_result::push<export_star_module_spec_state>(s_->coord_);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "'from'", token_->to_string());
	}

	using basic_state_visitor::operator();

private:
	export_star_from_state* s_;
	const lex::token& token_;
};

export_star_from_state::export_star_from_state(export_star_state* coord)
	: coord_(coord) {}

state_result export_star_from_state::process(parser& /*p*/,
											 const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result export_star_from_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	return accept_result::complete(nullptr);
}

class export_star_module_spec_state::visitor : public basic_state_visitor {
public:
	visitor(export_star_module_spec_state* s, const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::constant_value_token&) const {
		s_->coord_->set_module_specifier(token_);
		return state_result::push<export_star_semicolon_state>(s_->coord_);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "module specifier",
							 token_->to_string());
	}

	using basic_state_visitor::operator();

private:
	export_star_module_spec_state* s_;
	const lex::token& token_;
};

export_star_module_spec_state::export_star_module_spec_state(
	export_star_state* coord)
	: coord_(coord) {}

state_result export_star_module_spec_state::process(parser& /*p*/,
													const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result export_star_module_spec_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	return accept_result::complete(nullptr);
}

class export_star_semicolon_state::visitor : public basic_state_visitor {
public:
	visitor(export_star_semicolon_state* s, const lex::source_location& loc,
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
	export_star_semicolon_state* s_;
	const lex::token& token_;
};

export_star_semicolon_state::export_star_semicolon_state(
	export_star_state* coord)
	: coord_(coord) {}

state_result export_star_semicolon_state::process(parser& /*p*/,
												  const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result export_star_semicolon_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	throw std::logic_error(
		"export_star_semicolon_state does not push children");
}

std::optional<state_result> export_star_semicolon_state::on_eof() {
	return state_result::complete(nullptr);
}
