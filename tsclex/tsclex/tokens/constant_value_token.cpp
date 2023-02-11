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
#include <bitset>
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <tsccore/json.hpp>

using namespace tscc::lex::tokens;

constant_value_token::constant_value_token(std::u32string string_value)
	: value_(std::move(string_value)) {}

constant_value_token::constant_value_token(long long integer_value,
										   integer_base base)
	: value_(integer_data(integer_value, base)) {}

constant_value_token::constant_value_token(long double decimal_value)
	: value_(decimal_value) {}

constant_value_token::constant_value_token(long double decimal_value,
										   int scientific_notation_e,
										   bool upper)
	: value_(float_data(decimal_value, scientific_notation_e, upper)) {}

bool constant_value_token::operator==(
	const tscc::lex::tokens::constant_value_token& other) const {
	return value_ == other.value_;
}

bool constant_value_token::operator!=(
	const tscc::lex::tokens::constant_value_token& other) const {
	return value_ != other.value_;
}

std::string constant_value_token::to_string() const {
	static thread_local std::stringstream str;

	if (std::holds_alternative<std::u32string>(value_)) {
		return to_json_string(std::get<std::u32string>(value_));
	} else if (std::holds_alternative<integer_data>(value_)) {
		auto& d = std::get<integer_data>(value_);

		str.clear();
		str.seekp(0, std::ios::beg);
		str.str(std::string{});

		switch (d.base) {
			case integer_base::binary:
				str << "0b" << std::bitset<sizeof(d.value) * 8>(d.value);
			case integer_base::octal:
				str << "0o" << std::oct << d.value;
			case integer_base::hex:
				str << std::hex << "0x" << d.value;
				break;
			default:
				str << std::dec << d.value;
		}

		return str.str();
	} else {
		auto& d = std::get<float_data>(value_);

		str.clear();
		str.seekp(0, std::ios::beg);
		str.str(std::string{});

		constexpr auto precision =
			std::numeric_limits<decltype(d.value)>::max_digits10 + 2;
		if (d.scientific_exponent) {
			auto value =
				d.value / std::pow(10.0l, d.scientific_exponent->exponent);
			str << std::setprecision(precision - 3) << value
				<< (d.scientific_exponent->upper_case_e ? 'E' : 'e')
				<< d.scientific_exponent->exponent;
		} else {
			str << std::setprecision(precision) << d.value;
		}

		return str.str();
	}
}