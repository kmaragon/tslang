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

#include <tsccore/utf8.hpp>
#include <tsclex/token.hpp>

namespace tscc::parse::ast::detail {

template <typename Token>
const Token& token_as(const lex::token& token) noexcept {
	return static_cast<const Token&>(*token);
}

template <typename T>
struct lexeme_value_extractor<lex::tokens::identifier_token, T> {
	static_assert(std::is_convertible_v<const std::string&, T>,
				  "lex::tokens::identifier_token only supports std::string");

	struct not_supported_combination {};
	using getter = T (*)(const lex::token&);

	getter make_getter(const lex::tokens::identifier_token& token) const {
		return [](const lex::token& token) -> T {
			return token_as<lex::tokens::identifier_token>(token).id();
		};
	}
};

template <>
struct lexeme_value_extractor<lex::tokens::constant_value_token, std::string> {
	struct not_supported_combination {};
	using getter = std::string (*)(const lex::token&);

	getter make_getter(const lex::tokens::constant_value_token& token) const {
		if (!token.is_string())
			throw std::invalid_argument(
				"Constant Vaulue is not string as expected");

		return [](const lex::token& token) {
			return utf8_encode(
				*token_as<lex::tokens::constant_value_token>(token)
					 .string_value());
		};
	}
};

template <>
struct lexeme_value_extractor<lex::tokens::constant_value_token, tscc_big_int> {
	struct not_supported_combination {};
	using getter = tscc_big_int (*)(const lex::token&);

	getter make_getter(const lex::tokens::constant_value_token& token) const {
		if (!token.is_bigint())
			throw std::invalid_argument(
				"Constant Vaulue is not number as expected");

		return [](const lex::token& token) {
			return *token_as<lex::tokens::constant_value_token>(token)
						.integer_value();
		};
	}
};

template <>
struct lexeme_value_extractor<lex::tokens::constant_value_token, long double> {
	struct not_supported_combination {};
	using getter = long double (*)(const lex::token&);

	getter make_getter(const lex::tokens::constant_value_token& token) const {
		if (!token.is_bigint())
			throw std::invalid_argument(
				"Constant Vaulue is not number as expected");

		return [](const lex::token& token) {
			return *token_as<lex::tokens::constant_value_token>(token)
						.decimal_value();
		};
	}
};

}  // namespace tscc::parse::ast::detail