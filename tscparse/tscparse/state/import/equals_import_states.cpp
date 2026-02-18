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

#include "equals_import_states.hpp"
#include <stdexcept>
#include <tsclex/tokens/as_token.hpp>
#include <tsclex/tokens/assert_token.hpp>
#include <tsclex/tokens/close_paren_token.hpp>
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/dot_token.hpp>
#include <tsclex/tokens/from_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/open_paren_token.hpp>
#include <tsclex/tokens/require_token.hpp>
#include <tsclex/tokens/semicolon_token.hpp>
#include <tsclex/tokens/type_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"

namespace tscc::parse {

after_equals_state::after_equals_state(ast::import_node* node) : node_(node) {}

class after_equals_state::visitor : public basic_state_visitor {
public:
	visitor(after_equals_state* s,
			const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::require_token&) const {
		s_->node_->set_require_keyword(token_);
		return state_result::push<expect_require_paren_state>(s_->node_);
	}

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

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "'require' or identifier",
							 token_->to_string());
	}

private:
	state_result handle_identifier() const {
		s_->node_->add_entity_identifier(token_);
		return state_result::push<import_entity_state>(s_->node_);
	}

	after_equals_state* s_;
	const lex::token& token_;
};

state_result after_equals_state::process(parser& /*p*/,
										 const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result after_equals_state::accept_child(std::unique_ptr<ast::ast_node>) {
	return accept_result::complete(nullptr);
}

expect_require_paren_state::expect_require_paren_state(ast::import_node* node)
	: node_(node) {}

class expect_require_paren_state::visitor : public basic_state_visitor {
public:
	visitor(expect_require_paren_state* s,
			const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::open_paren_token&) const {
		s_->node_->set_require_open_paren(token_);
		return state_result::push<after_require_open_state>(s_->node_);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "'('", token_->to_string());
	}

private:
	expect_require_paren_state* s_;
	const lex::token& token_;
};

state_result expect_require_paren_state::process(parser& /*p*/,
												 const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result expect_require_paren_state::accept_child(
	std::unique_ptr<ast::ast_node>) {
	return accept_result::complete(nullptr);
}

after_require_open_state::after_require_open_state(ast::import_node* node)
	: node_(node) {}

class after_require_open_state::visitor : public basic_state_visitor {
public:
	visitor(after_require_open_state* s,
			const lex::source_location& loc,
			const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::constant_value_token&) const {
		s_->node_->set_require_module_specifier(token_);
		return state_result::push<after_require_module_state>(s_->node_);
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "module specifier", token_->to_string());
	}

private:
	after_require_open_state* s_;
	const lex::token& token_;
};

state_result after_require_open_state::process(parser& /*p*/,
											   const lex::token& token) {
	return token.visit(visitor{this, token.location(), token});
}

accept_result after_require_open_state::accept_child(
	std::unique_ptr<ast::ast_node>) {
	return accept_result::complete(nullptr);
}

after_require_module_state::after_require_module_state(ast::import_node* node)
	: node_(node) {}

class after_require_module_state::expect_close_visitor
	: public basic_state_visitor {
public:
	expect_close_visitor(after_require_module_state* s,
						 const lex::source_location& loc,
						 const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::close_paren_token&) const {
		s_->node_->set_require_close_paren(token_);
		s_->closed_ = true;
		return state_result::stay();
	}

	[[noreturn]] state_result operator()(
		const lex::tokens::basic_token&) const {
		throw expected_token(location, "')'", token_->to_string());
	}

private:
	after_require_module_state* s_;
	const lex::token& token_;
};

class after_require_module_state::after_close_visitor
	: public basic_state_visitor {
public:
	after_close_visitor(after_require_module_state* s,
						const lex::source_location& loc,
						const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::semicolon_token&) const {
		s_->node_->set_semicolon(token_);
		return state_result::complete(nullptr);
	}

	// ASI
	state_result operator()(const lex::tokens::basic_token&) const {
		return state_result::complete(nullptr).reprocess();
	}

private:
	after_require_module_state* s_;
	const lex::token& token_;
};

state_result after_require_module_state::process(parser& /*p*/,
												 const lex::token& token) {
	if (closed_)
		return token.visit(after_close_visitor{this, token.location(), token});
	return token.visit(expect_close_visitor{this, token.location(), token});
}

accept_result after_require_module_state::accept_child(
	std::unique_ptr<ast::ast_node>) {
	return accept_result::complete(nullptr);
}

std::optional<state_result> after_require_module_state::on_eof() {
	if (closed_)
		return state_result::complete(nullptr);
	return std::nullopt;
}

import_entity_state::import_entity_state(ast::import_node* node)
	: node_(node) {}

class import_entity_state::after_id_visitor : public basic_state_visitor {
public:
	after_id_visitor(import_entity_state* s,
					 const lex::source_location& loc,
					 const lex::token& token) noexcept
		: basic_state_visitor(s, loc), s_(s), token_(token) {}

	state_result operator()(const lex::tokens::dot_token&) const {
		s_->node_->add_entity_dot(token_);
		s_->expecting_id_ = true;
		return state_result::stay();
	}

	state_result operator()(const lex::tokens::semicolon_token&) const {
		s_->node_->set_semicolon(token_);
		return state_result::complete(nullptr);
	}

	// ASI
	state_result operator()(const lex::tokens::basic_token&) const {
		return state_result::complete(nullptr).reprocess();
	}

private:
	import_entity_state* s_;
	const lex::token& token_;
};

class import_entity_state::expect_id_visitor : public basic_state_visitor {
public:
	expect_id_visitor(import_entity_state* s,
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
		s_->node_->add_entity_identifier(token_);
		s_->expecting_id_ = false;
		return state_result::stay();
	}

	import_entity_state* s_;
	const lex::token& token_;
};

state_result import_entity_state::process(parser& /*p*/,
										  const lex::token& token) {
	if (expecting_id_)
		return token.visit(expect_id_visitor{this, token.location(), token});
	return token.visit(after_id_visitor{this, token.location(), token});
}

accept_result import_entity_state::accept_child(
	std::unique_ptr<ast::ast_node>) {
	return accept_result::complete(nullptr);
}

std::optional<state_result> import_entity_state::on_eof() {
	if (!expecting_id_)
		return state_result::complete(nullptr);
	return std::nullopt;
}

}  // namespace tscc::parse
