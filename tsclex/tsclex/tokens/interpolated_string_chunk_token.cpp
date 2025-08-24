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

#include "interpolated_string_chunk_token.hpp"

#include "tsccore/json.hpp"
#include "tsccore/utf8.hpp"

using namespace tscc::lex::tokens;

interpolated_string_chunk_token::interpolated_string_chunk_token(
	const std::u32string& chunk)
	: chunk_(chunk) {}

bool interpolated_string_chunk_token::operator==(
	const tscc::lex::tokens::interpolated_string_chunk_token& other) const {
	return true;
}

bool interpolated_string_chunk_token::operator!=(
	const tscc::lex::tokens::interpolated_string_chunk_token& other) const {
	return false;
}

const std::u32string& interpolated_string_chunk_token::value() const noexcept {
	return chunk_;
}

std::string interpolated_string_chunk_token::to_string() const {
	return to_json_string(chunk_, 0);
}