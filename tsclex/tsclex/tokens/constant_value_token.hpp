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

#pragma once

#include <optional>
#include <string>
#include <variant>
#include "basic_token.hpp"

namespace tscc::lex::tokens {

enum class integer_base { binary, octal, decimal, hex };

/**
 * @brief A typescript token that represents a constant value
 */
class constant_value_token : public basic_token {
public:
	constant_value_token(std::u32string string_value, char quote_char);
	constant_value_token(long long integer_value, integer_base base);
	constant_value_token(long double decimal_value);
	constant_value_token(long double decimal_value,
						 int scientific_notation_e,
						 bool upper = false);

	bool operator==(const constant_value_token& other) const;
	bool operator!=(const constant_value_token& other) const;

	std::string to_string() const override;

	std::optional<long long> integer_value() const noexcept;
	std::optional<long double> decimal_value() const noexcept;
	std::optional<std::u32string_view> string_value() const noexcept;

private:
	struct string_data {
		std::u32string value;
		char quote;

		string_data(std::u32string string_value, char quote)
			: value(std::move(string_value)), quote(quote) {}

		bool operator==(const string_data& other) const noexcept {
			return value == other.value && quote == other.quote;
		}

		bool operator!=(const string_data& other) const noexcept {
			return !operator==(other);
		}
	};

	struct integer_data {
		long long value;
		integer_base base;

		constexpr integer_data(long long v, integer_base b) noexcept
			: value(v), base(b) {}

		constexpr bool operator==(const integer_data& other) const noexcept {
			return value == other.value && base == other.base;
		}
		constexpr bool operator!=(const integer_data& other) const noexcept {
			return !operator==(other);
		}
	};

	struct float_representation_flags {
		int exponent;
		bool upper_case_e;

		constexpr float_representation_flags(int exp, bool upper) noexcept
			: exponent(exp), upper_case_e(upper) {}

		constexpr bool operator==(
			const float_representation_flags& other) const noexcept {
			return exponent == other.exponent &&
				   upper_case_e == other.upper_case_e;
		}
		constexpr bool operator!=(
			const float_representation_flags& other) const noexcept {
			return !operator==(other);
		}
	};

	struct float_data {
		long double value;
		std::optional<float_representation_flags> scientific_exponent;

		constexpr float_data(long double v) noexcept : value(v) {}
		constexpr float_data(long double v, int exp, bool upper) noexcept
			: value(v) {
			scientific_exponent.emplace(exp, upper);
		}

		constexpr bool operator==(const float_data& other) const noexcept {
			return value == other.value &&
				   scientific_exponent == other.scientific_exponent;
		}
		constexpr bool operator!=(const float_data& other) const noexcept {
			return !operator==(other);
		}
	};

	std::variant<string_data, integer_data, float_data> value_;
};

}  // namespace tscc::lex::tokens