// TSCC - a Typescript Compiler
// Copyright (c) 2025. Keef Aragon
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#include "conflict_marker_trivia_token.hpp"
#include <tsccore/json.hpp>

using namespace tscc::lex::tokens;

conflict_marker_trivia_token::conflict_marker_trivia_token(
	char prefix_char,
	const std::u32string& comment_body)
	: body_(comment_body), prefix_(prefix_char) {}

bool conflict_marker_trivia_token::operator==(
	const tscc::lex::tokens::conflict_marker_trivia_token& other) const {
	return prefix_ == other.prefix_ && body_ == other.body_;
}

bool conflict_marker_trivia_token::operator!=(
	const tscc::lex::tokens::conflict_marker_trivia_token& other) const {
	return prefix_ != other.prefix_ || body_ != other.body_;
}

std::string conflict_marker_trivia_token::to_string() const {
	std::string result(static_cast<std::string::size_type>(7), prefix_);
	if (!body_.empty()) {
		result.push_back(' ');
		result.append(to_json_string(body_, 0));
	}

	return result;
}