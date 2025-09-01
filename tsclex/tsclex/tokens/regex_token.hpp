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

#include <cstdint>
#include <tsccore/regex/regular_expression.hpp>
#include "basic_token.hpp"

namespace tscc::lex::tokens {
class regex_token : public basic_token {
public:
	enum class flags : std::uint8_t {
		none = 0,
		ignore_case = 1 << 0,
		global = 1 << 1,
		multiline = 1 << 2,
		dot_all = 1 << 3,
		unicode = 1 << 4,
		sticky = 1 << 5
	};

	regex_token(tsccore::regex::regular_expression&& expression, flags flags = flags::none) noexcept;

	bool operator==(const regex_token& other) const;
	bool operator!=(const regex_token& other) const;

	std::string to_string() const override;

	flags get_flags() const noexcept;

private:
	tsccore::regex::regular_expression expr_;
	flags flags_;
};

constexpr regex_token::flags operator|(regex_token::flags lhs, regex_token::flags rhs) noexcept {
	return static_cast<regex_token::flags>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
}

constexpr regex_token::flags operator&(regex_token::flags lhs, regex_token::flags rhs) noexcept {
	return static_cast<regex_token::flags>(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
}

constexpr bool has_flag(regex_token::flags value, regex_token::flags flag) noexcept {
	return (value & flag) == flag;
}

}  // namespace tscc::lex::tokens