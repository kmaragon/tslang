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

#include "jsdoc_token.hpp"
#include <sstream>

using namespace tscc::lex::tokens;

jsdoc_token::jsdoc_token(const std::span<std::u32string>& comment_lines) {
	// TODO: implement parsing jsdoc
}

bool jsdoc_token::operator==(
	const tscc::lex::tokens::jsdoc_token& other) const {
	return false;
}

bool jsdoc_token::operator!=(
	const tscc::lex::tokens::jsdoc_token& other) const {
	return !operator==(other);
}

std::string jsdoc_token::to_string() const {
	throw std::system_error(std::make_error_code(std::errc::not_supported),
							"JSDoc type scheme not built yet");
}