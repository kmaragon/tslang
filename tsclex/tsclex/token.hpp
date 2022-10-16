/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2022. Keef Aragon
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

#include <optional>
#include <variant>
#include "source_location.hpp"
#include "tokens/comment_token.hpp"
#include "tokens/shebang_token.hpp"

namespace tscc::lex {

/**
 * \brief An exception thrown when trying to extract the basic_token from an
 * undefined token
 */
class token_undefined : public std::exception {
public:
	token_undefined() noexcept = default;

	const char* what() const noexcept override;
};

/**
 * @brief A container for any of the possible basic_tokens
 */
class token {
public:
	/**
	 * @brief Get a reference to the underlying basic_token
	 */
	const tokens::basic_token& operator*() const noexcept;

	/**
	 * @brief Get a reference to the underlying basic_token
	 */
	tokens::basic_token& operator*() noexcept;

	/**
	 * @brief Get a pointer to the underlying basic_token
	 */
	const tokens::basic_token* operator->() const noexcept;

	/**
	 * @brief Get a pointer to the underlying basic_token
	 */
	tokens::basic_token* operator->() noexcept;

	/**
	 * @brief Visit the token by its specific type
	 *
	 * The visitor must provide a matching overload for every type
	 * of token available in the \see tsc::lex::tokens namespace.
	 * This can either be done explicitly or by providing an overload
	 * for the generic base basic_token.
	 *
	 * For example:
	 * @example
	 * @code c++
	 * struct comment_token_visitor
	 * {
	 *   int operator()(const tsc::lex::tokens::comment_token& token) const {
	 *     ...
	 *   }
	 *
	 *   int operator()(const tsc::lex::tokens::basic_token&) const {
	 *     return 0;
	 *   }
	 * };
	 * @endcode
	 */
	template <typename T>
	decltype(auto) visit(T&& visitor) const {
		if (!token_)
			throw token_undefined();
		return std::visit(std::forward<T>(visitor), *token_);
	}

	/**
	 * @brief Get the location where the token is defined
	 */
	const source_location& location() const noexcept;

	/**
	 * \brief Emplace a new value into the token
	 */
	template <typename Token, typename... Args>
	void emplace_token(const source_location& location, Args&&... args) {
		if (token_) {
			token_->emplace<Token>(std::forward<Args>(args)...);
		} else {
			token_.emplace(all_tokens_t{Token{std::forward<Args>(args)...}});
		}
		location_ = location;
	}

	/**
	 * \brief Set the token to an undefined value
	 */
	void undefine();

	/**
	 * \brief Compare the token for equality vs another token
	 */
	bool operator==(const token& other) const;

	/**
	 * \brief Compare the token for inequality vs another token
	 */
	bool operator!=(const token& other) const;

private:
	using all_tokens_t = std::variant<tokens::comment_token,
									  tokens::shebang_token>;

	source_location location_;
	std::optional<all_tokens_t> token_;
};

}  // namespace tscc::lex