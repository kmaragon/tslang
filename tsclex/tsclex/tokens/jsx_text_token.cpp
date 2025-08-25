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

#include "jsx_text_token.hpp"
#include <tsccore/utf8.hpp>
#include "tsccore/xml.hpp"

using namespace tscc::lex::tokens;

jsx_text_token::jsx_text_token(const std::u32string& text)
	: text_(xml_decode(text)) {}

bool jsx_text_token::operator==(
	const jsx_text_token& other) const {
	return text_ == other.text_;
}

bool jsx_text_token::operator!=(
	const jsx_text_token& other) const {
	return !operator==(other);
}

const std::u32string& jsx_text_token::text() const noexcept {
	return text_;
}

std::string jsx_text_token::to_string() const {
	return utf8_encode(xml_encode(text_));
}