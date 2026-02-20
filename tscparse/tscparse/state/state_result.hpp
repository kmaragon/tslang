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

#pragma once

#include <memory>
#include "../ast/ast_node.hpp"
#include "parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Result of a parser state processing a token
 *
 * Represents the outcome of a state transition:
 * - stay: remain in current state
 * - push: enter a child state
 * - complete: this state is done, return AST to parent
 *
 * By default, the triggering token is consumed. Call reprocess()
 * to indicate the next handler (child or parent) should see the token.
 */
class state_result {
public:
	state_result(state_result&&) noexcept = default;
	state_result& operator=(state_result&&) noexcept = default;
	state_result(const state_result&) = delete;
	state_result& operator=(const state_result&) = delete;

	[[nodiscard]] static state_result stay();

	[[nodiscard]] static state_result push(std::unique_ptr<parser_state> child);

	template <typename State, typename... Args>
	[[nodiscard]] static state_result push(Args&&... args) {
		return push(std::make_unique<State>(std::forward<Args>(args)...));
	}

	[[nodiscard]] static state_result complete(
		std::unique_ptr<ast::ast_node> node);

	template <typename Node, typename... Args>
	[[nodiscard]] static state_result complete(Args&&... args) {
		return complete(std::make_unique<Node>(std::forward<Args>(args)...));
	}

	[[nodiscard]] state_result reprocess() &&;

	[[nodiscard]] bool should_reprocess() const noexcept;
	[[nodiscard]] bool is_stay() const noexcept;
	[[nodiscard]] bool is_push() const noexcept;
	[[nodiscard]] bool is_complete() const noexcept;

	[[nodiscard]] std::unique_ptr<parser_state> take_child() &&;
	[[nodiscard]] std::unique_ptr<ast::ast_node> take_node() &&;

private:
	state_result() = default;

	enum class action { stay, push, complete };

	action action_ = action::stay;
	bool reprocess_ = false;
	std::unique_ptr<parser_state> child_;
	std::unique_ptr<ast::ast_node> node_;
};

/**
 * \brief Result of a parser state's accept_child method
 *
 * When a child state completes, the parent receives the child's AST
 * via accept_child(). The parent returns accept_result to indicate:
 * - stay: child absorbed, parent ready for more tokens
 * - complete: parent is also done, here's the combined AST
 *
 * Note: reprocess is handled externally by the parser from the
 * triggering process() call, not from accept_child.
 */
class accept_result {
public:
	// Move-only (contains unique_ptr)
	accept_result(accept_result&&) noexcept = default;
	accept_result& operator=(accept_result&&) noexcept = default;
	accept_result(const accept_result&) = delete;
	accept_result& operator=(const accept_result&) = delete;

	// Factory: absorbed child, ready for more tokens
	[[nodiscard]] static accept_result stay();

	// Factory: parent is also complete, here's the combined AST
	[[nodiscard]] static accept_result complete(
		std::unique_ptr<ast::ast_node> node);

	// Factory: complete with in-place node construction
	template <typename Node, typename... Args>
	[[nodiscard]] static accept_result complete(Args&&... args) {
		return complete(std::make_unique<Node>(std::forward<Args>(args)...));
	}

	// Queries
	[[nodiscard]] bool is_stay() const noexcept;
	[[nodiscard]] bool is_complete() const noexcept;

	// Extraction (rvalue-qualified, destructive)
	[[nodiscard]] std::unique_ptr<ast::ast_node> take_node() &&;

private:
	accept_result() = default;

	enum class action { stay, complete };

	action action_ = action::stay;
	std::unique_ptr<ast::ast_node> node_;
};

}  // namespace tscc::parse::state
