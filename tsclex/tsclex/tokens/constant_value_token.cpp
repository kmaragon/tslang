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

#include "constant_value_token.hpp"
#include <tsccore/json.hpp>

using namespace tscc::lex::tokens;

constant_value_token::constant_value_token(std::u32string string_value)
	: value_(std::move(string_value)) {}

constant_value_token::constant_value_token(long long integer_value)
	: value_(integer_value) {}

constant_value_token::constant_value_token(long double decimal_value)
	: value_(decimal_value) {}

bool constant_value_token::operator==(
	const tscc::lex::tokens::constant_value_token& other) const {
	return value_ == other.value_;
}

bool constant_value_token::operator!=(
	const tscc::lex::tokens::constant_value_token& other) const {
	return value_ != other.value_;
}

std::string constant_value_token::to_string() const
{
	if (std::holds_alternative<std::u32string>(value_)) {
		return to_json_string(std::get<std::u32string>(value_));
	} else if (std::holds_alternative<long long>(value_)) {
		return std::to_string(std::get<long long>(value_));
	} else {
		return std::to_string(std::get<long double>(value_));
	}
}