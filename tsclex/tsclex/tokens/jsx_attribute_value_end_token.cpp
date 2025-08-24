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

#include "jsx_attribute_value_end_token.hpp"

using namespace tscc::lex::tokens;

jsx_attribute_value_end_token::jsx_attribute_value_end_token(value_type type)
	: type_(type) {}

bool jsx_attribute_value_end_token::operator==(
	const jsx_attribute_value_end_token& other) const {
	return type_ == other.type_;
}

bool jsx_attribute_value_end_token::operator!=(
	const jsx_attribute_value_end_token& other) const {
	return !operator==(other);
}

jsx_attribute_value_end_token::value_type jsx_attribute_value_end_token::type() const noexcept {
	return type_;
}

std::string jsx_attribute_value_end_token::to_string() const {
	switch (type_) {
		case value_type::string:
			return "\"";  // Could be " or ' but " is more common
		case value_type::expression:
			return "}";
		default:
			return "?";
	}
}