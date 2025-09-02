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

#include "regex_token.hpp"

#include <complex>

#include "tsccore/utf8.hpp"

using namespace tscc::lex::tokens;

regex_token::regex_token(tsccore::regex::regular_expression&& expression,
						 flags flags) noexcept
	: expr_(std::move(expression)), flags_(flags) {}

bool regex_token::operator==(
	const tscc::lex::tokens::regex_token& other) const {
	return expr_ == other.expr_ && flags_ == other.flags_;
}

bool regex_token::operator!=(
	const tscc::lex::tokens::regex_token& other) const {
	return expr_ != other.expr_;
}

std::string regex_token::to_string() const {
	std::u32string result;
	result.reserve(expr_.string_size());
	expr_.to_string(result);
	auto res = utf8_encode(result);

	res.reserve(res.size() + 2 +
				__builtin_popcountll(static_cast<unsigned long long>(flags_)));
	res.insert(res.begin(), 1, '/');
	res.push_back('/');

	if (has_flag(flags_, flags::ignore_case))
		res.push_back('i');
	if (has_flag(flags_, flags::global))
		res.push_back('g');
	if (has_flag(flags_, flags::multiline))
		res.push_back('m');
	if (has_flag(flags_, flags::dot_all))
		res.push_back('s');
	if (has_flag(flags_, flags::unicode))
		res.push_back('u');
	if (has_flag(flags_, flags::unicode_sets))
		res.push_back('v');
	if (has_flag(flags_, flags::sticky))
		res.push_back('y');
	return res;
}

regex_token::flags regex_token::get_flags() const noexcept {
	return flags_;
}
