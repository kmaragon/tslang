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

#include <string>
#include <variant>
#include "basic_token.hpp"

namespace tscc::lex::tokens {

/**
 * @brief A typescript token that represents a comment
 */
class constant_value_token : public basic_token {
public:
	constant_value_token(std::u32string string_value);
	constant_value_token(long long integer_value);
	constant_value_token(long double decimal_value);

	bool operator==(const constant_value_token& other) const;
	bool operator!=(const constant_value_token& other) const;

	std::string to_string() const override;

private:
	std::variant<std::u32string, long long, long double> value_;
};

}  // namespace tscc::lex::tokens