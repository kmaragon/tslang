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

#include "multiline_comment_token.hpp"
#include <sstream>

using namespace tscc::lex::tokens;

multiline_comment_token::multiline_comment_token(
	std::vector<std::wstring> comment_lines)
	: lines_(std::move(comment_lines)) {}

bool multiline_comment_token::operator==(
	const tscc::lex::tokens::multiline_comment_token& other) const {
	if (lines_.size() != other.lines_.size()) {
		return false;
	}

	for (std::size_t i = 0; i < lines_.size(); i++) {
		if (lines_[i] != other.lines_[i])
			return false;
	}

	return true;
}

bool multiline_comment_token::operator!=(
	const tscc::lex::tokens::multiline_comment_token& other) const {
	return !operator==(other);
}

std::wstring multiline_comment_token::to_string() const {
	std::wstringstream out;
	out << "/*";
	for (auto& l : lines_) {
		out << l << "\n";
	}

	out << "*/";
	return out.str();
}