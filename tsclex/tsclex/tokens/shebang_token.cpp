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

#include "shebang_token.hpp"

using namespace tscc::lex::tokens;

shebang_token::shebang_token(std::wstring command)
	: cmd_(std::move(command)) {}

bool shebang_token::operator==(
	const tscc::lex::tokens::shebang_token& other) const {
	return cmd_ == other.cmd_;
}

bool shebang_token::operator!=(
	const tscc::lex::tokens::shebang_token& other) const {
	return cmd_ != other.cmd_;
}

std::wstring shebang_token::to_string() const {
	return L"#!" + cmd_;
}