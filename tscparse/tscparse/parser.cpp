/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2025. Keef Aragon
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

#include "parser.hpp"
#include <cassert>
#include <tsclex/tokens/abstract_token.hpp>
#include <tsclex/tokens/async_token.hpp>
#include <tsclex/tokens/comment_token.hpp>
#include <tsclex/tokens/jsdoc_token.hpp>
#include <tsclex/tokens/multiline_comment_token.hpp>
#include <tsclex/tokens/newline_token.hpp>
#include <tsclex/tokens/private_token.hpp>
#include <tsclex/tokens/protected_token.hpp>
#include <tsclex/tokens/public_token.hpp>
#include <tsclex/tokens/readonly_token.hpp>
#include <tsclex/tokens/static_token.hpp>
#include "error/expected_token.hpp"
#include "error/unexpected_end_of_text.hpp"
#include "state/module_scope_state.hpp"
#include "state/state_result.hpp"

using namespace tscc;
using namespace tscc::parse;

parser::parser(lex::lexer& lexer, trivia_index* trivia_idx)
	: lexer_(lexer),
	  token_iter_(lexer.begin()),
	  token_end_(lexer.end()),
	  trivia_index_(trivia_idx) {}

std::unique_ptr<ast::source_file_node> parser::parse() {
	auto root = std::unique_ptr<ast::source_file_node>(
		new ast::source_file_node(lexer_.source()));
	state_stack_.emplace_back(
		std::make_unique<state::module_scope_state>(root.get()));

	collect_trivia();

	while (!at_token_end()) {
		const auto& token = current_token();
		auto result = state_stack_.back()->process(*this, token);

		if (!result.should_reprocess()) {
			advance_token();
			collect_trivia();
		}

		if (result.is_stay()) {
			continue;
		}

		if (result.is_push()) {
			state_stack_.emplace_back(std::move(result).take_child());
			continue;
		}

		if (result.is_complete()) {
			handle_complete(std::move(result));
		}
	}

	// Handle EOF: unwind any states that can complete at EOF
	while (state_stack_.size() > 1) {
		if (auto eof_result = state_stack_.back()->on_eof()) {
			handle_complete(std::move(*eof_result));
		} else {
			throw unexpected_end_of_text(last_location_);
		}
	}

	// Flush any remaining trivia as orphaned
	if (trivia_index_) {
		for (auto& tok : pending_trivia_) {
			trivia_index_->emplace(std::move(tok), nullptr,
								   trivia_ref::relationship::orphaned);
		}
	}
	pending_trivia_.clear();

	return root;
}

void parser::advance_token() {
	if (token_iter_ != token_end_) {
		last_location_ = token_iter_->location();
		++token_iter_;
	}
}

const lex::token& parser::current_token() const {
	return *token_iter_;
}

bool parser::at_token_end() const {
	return token_iter_ == token_end_;
}

lex::token parser::consume_token() {
	lex::token tok = *token_iter_;
	advance_token();
	return tok;
}

void parser::collect_trivia() {
	while (!at_token_end()) {
		bool is_trivia = current_token().visit([&](const auto& tok) {
			using T = std::decay_t<decltype(tok)>;
			if constexpr (std::is_same_v<T, lex::tokens::newline_token> ||
						  std::is_same_v<T, lex::tokens::comment_token> ||
						  std::is_same_v<
							  T, lex::tokens::multiline_comment_token> ||
						  std::is_same_v<T, lex::tokens::jsdoc_token>) {
				pending_trivia_.emplace_back(std::move(*token_iter_));
				return true;
			}
			return false;
		});

		if (!is_trivia)
			break;
		advance_token();
	}
}

void parser::flush_trivia(ast::ast_node* node) {
	if (trivia_index_ && node) {
		for (auto& tok : pending_trivia_) {
			trivia_index_->emplace(std::move(tok), node,
								   trivia_ref::relationship::leading);
		}
	}
	pending_trivia_.clear();
}

void parser::handle_complete(state::state_result result) {
	auto node = std::move(result).take_node();
	state_stack_.pop_back();

	while (true) {
		assert(!state_stack_.empty() &&
			   "state stack unexpectedly empty during handle_complete");
		if (state_stack_.empty()) {
			return;
		}

		flush_trivia(node.get());
		auto accept = state_stack_.back()->accept_child(std::move(node));
		if (accept.is_stay()) {
			return;
		}

		node = std::move(accept).take_node();
		state_stack_.pop_back();
	}
}

std::optional<lex::token> parser::try_consume_modifier() {
	struct modifier_visitor {
		parser& p;

		std::optional<lex::token> operator()(const lex::tokens::public_token&) {
			return p.consume_token();
		}

		std::optional<lex::token> operator()(
			const lex::tokens::private_token&) {
			return p.consume_token();
		}

		std::optional<lex::token> operator()(
			const lex::tokens::protected_token&) {
			return p.consume_token();
		}

		std::optional<lex::token> operator()(const lex::tokens::static_token&) {
			return p.consume_token();
		}

		std::optional<lex::token> operator()(
			const lex::tokens::readonly_token&) {
			return p.consume_token();
		}

		std::optional<lex::token> operator()(
			const lex::tokens::abstract_token&) {
			return p.consume_token();
		}

		std::optional<lex::token> operator()(const lex::tokens::async_token&) {
			return p.consume_token();
		}

		// Not a modifier
		std::optional<lex::token> operator()(const lex::tokens::basic_token&) {
			return std::nullopt;
		}
	};

	return current_token().visit(modifier_visitor{*this});
}

std::vector<lex::token> parser::parse_modifiers() {
	std::vector<lex::token> modifiers;

	while (auto modifier = try_consume_modifier()) {
		modifiers.emplace_back(std::move(*modifier));
	}

	return modifiers;
}

template <typename TokenType>
lex::token parser::expect_token(std::string_view token_name) {
	if (at_token_end()) {
		throw expected_token(last_location_, token_name, "EOF");
	}

	if (!current_token().is<TokenType>()) {
		throw expected_token(current_token().location(),
							 token_name,
							 current_token()->to_string());
	}

	return consume_token();
}
