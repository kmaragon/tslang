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

#include <tsclex/token.hpp>

namespace tscc::parse::ast {

namespace detail {

template <typename Token, typename T>
struct lexeme_value_extractor {
	struct not_supported_combination {};
	using getter = T (*)(const lex::token& token);

	getter make_getter(const Token&) const {
		throw std::invalid_argument("Unsupported getter");
	}
};

template <typename T>
struct lexeme_token_value_visitor {
	template <typename Token>
	decltype(auto) operator()(const Token& token) const {
		lexeme_value_extractor<Token, T> ext;
		return ext.make_getter(token);
	}
};

}  // namespace detail

/**
 * \brief The underlying value in a token
 *
 * This isn't taken from the pure text but instead represents the part of the
 * lexeme that's important in the AST context. So in a class definition, the
 * name will be a lexeme where the original token is accessible but it's also
 * just as easy to get the actual class name without going through the complex
 * lexical token enumeration to understand how.
 */
template <typename T>
class lexeme {
	const lex::token* token_;
	T (*value_)(const lex::token&);

public:
	/**
	 * \brief Default Constructor
	 */
	lexeme() : token_(nullptr), value_(nullptr) {}

	/**
	 * \brief Construct the lexeme for the provided token and getter
	 *
	 * \param token the token the lexeme is for
	 * \param fn the function to get the lexeme value from the token
	 */
	template <typename Fn>
	lexeme(
		const lex::token* token,
		std::enable_if_t<std::is_convertible_v<Fn, decltype(value_)>, Fn>&& fn)
		: token_(token), value_(fn) {}

	/**
	 * \brief Construct the lexeme for a provided token using the default getter
	 *
	 * \param token the token the lexeme is for
	 */
	lexeme(const lex::token* token) : token_(token), value_(nullptr) {
		if (token_ != nullptr) {
			detail::lexeme_token_value_visitor<T> visitor;
			value_ = token_->visit(visitor);
		}
	}

	/**
	 * \brief Whether the token is constructed and valid
	 */
	bool valid() const noexcept {
		return token_ != nullptr && value_ != nullptr;
	}

	/**
	 * \brief Convenience wrapper for validity
	 */
	operator bool() const noexcept { return valid(); }

	/**
	 * \brief Extract the lexeme value
	 */
	T value() const { return value_(*token_); }

	/**
	 * \brief Convenience operator for the lexeme value
	 */
	operator T() const { return value(); }

	/**
	 * \brief Get the token
	 */
	const lex::token* token() const noexcept { return token_; }

	/**
	 * \brief Convenience wrapper for the token
	 */
	const lex::token* operator->() const noexcept { return token_; }
};

}  // namespace tscc::parse::ast

#include "lexeme_visitors.ipp"