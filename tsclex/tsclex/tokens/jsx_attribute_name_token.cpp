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

#include "jsx_attribute_name_token.hpp"
#include <tsccore/utf8.hpp>

using namespace tscc::lex::tokens;

jsx_attribute_name_token::jsx_attribute_name_token(const std::u32string& name)
	: name_(name) {}

bool jsx_attribute_name_token::operator==(
	const jsx_attribute_name_token& other) const {
	return name_ == other.name_;
}

bool jsx_attribute_name_token::operator!=(
	const jsx_attribute_name_token& other) const {
	return !operator==(other);
}

const std::u32string& jsx_attribute_name_token::name() const noexcept {
	return name_;
}

std::string jsx_attribute_name_token::to_string() const {
	return utf8_encode(name_);
}