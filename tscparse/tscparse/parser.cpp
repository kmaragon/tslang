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

parser::iterator::iterator(parser* p) : parser_(p), current_node_(nullptr) {
	advance();
}

void parser::iterator::advance() {
	if (parser_->at_token_end()) {
		current_node_ = nullptr;
		return;
	}

	current_node_ = parser_->parse_top_level_element();
}

parser::iterator& parser::iterator::operator++() {
	advance();
	return *this;
}

bool parser::iterator::operator==(const iterator& other) const {
	// For move-only iterators, this is rarely used
	// Two iterators are the same if they refer to the same object
	return this == &other;
}

bool parser::iterator::operator!=(const iterator& other) const {
	return !(*this == other);
}

bool parser::iterator::operator==(sentinel) const {
	return current_node_ == nullptr;
}

bool parser::iterator::operator!=(sentinel) const {
	return current_node_ != nullptr;
}

parser::parser(lex::lexer& lexer, trivia_index* trivia_idx)
	: token_iter_(lexer.begin()),
	  token_end_(lexer.end()),
	  trivia_index_(trivia_idx) {
	state_stack_.emplace_back(std::make_unique<state::module_scope_state>());
}

parser::iterator parser::begin() {
	return iterator{this};
}

parser::sentinel parser::end() {
	return sentinel{};
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
	pending_trivia_.clear();

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

void parser::attach_trivia_to_node(ast::ast_node* node,
								   trivia_ref::relationship rel) {
	if (trivia_index_ && node) {
		for (const auto& tok : pending_trivia_) {
			trivia_index_->emplace(tok, node, rel);
		}
	}
	pending_trivia_.clear();
}

std::unique_ptr<ast::ast_node> parser::parse_top_level_element() {
	collect_trivia();

	if (at_token_end()) {
		if (state_stack_.size() > 1) {
			if (auto eof_result = state_stack_.back()->on_eof()) {
				return handle_complete(std::move(*eof_result));
			}
			throw unexpected_end_of_text(last_location_);
		}
		return nullptr;
	}

	while (!state_stack_.empty()) {
		const auto& token = current_token();
		auto result = state_stack_.back()->process(*this, token);

		if (!result.should_reprocess()) {
			advance_token();
			collect_trivia();
		}

		if (result.is_stay()) {
			if (at_token_end()) {
				if (state_stack_.size() > 1) {
					if (auto eof_result = state_stack_.back()->on_eof()) {
						return handle_complete(std::move(*eof_result));
					}
					throw unexpected_end_of_text(last_location_);
				}
				return nullptr;
			}
			continue;
		}

		if (result.is_push()) {
			state_stack_.emplace_back(std::move(result).take_child());
			if (at_token_end()) {
				if (state_stack_.size() > 1) {
					if (auto eof_result = state_stack_.back()->on_eof()) {
						return handle_complete(std::move(*eof_result));
					}
					throw unexpected_end_of_text(last_location_);
				}
				return nullptr;
			}
			continue;
		}

		if (result.is_complete()) {
			auto completed_node = handle_complete(std::move(result));
			if (completed_node) {
				return completed_node;
			}
		}

		if (at_token_end()) {
			if (state_stack_.size() > 1) {
				if (auto eof_result = state_stack_.back()->on_eof()) {
					return handle_complete(std::move(*eof_result));
				}
				throw unexpected_end_of_text(last_location_);
			}
			return nullptr;
		}
	}

	return nullptr;
}

std::unique_ptr<ast::ast_node> parser::handle_complete(
	state::state_result result) {
	auto node = std::move(result).take_node();
	state_stack_.pop_back();

	while (true) {
		if (state_stack_.empty() || state_stack_.size() == 1) {
			return node;
		}

		auto accept = state_stack_.back()->accept_child(std::move(node));
		if (accept.is_stay()) {
			return nullptr;
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