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

#include "error.hpp"

using namespace tsccore::regex;

// regex_error base class implementation
regex_error::regex_error(size_t offset) noexcept : offset_(offset) {}

size_t regex_error::offset() const noexcept {
	return offset_;
}

// Specific error implementations
invalid_regular_expression::invalid_regular_expression(size_t offset) noexcept
	: regex_error(offset) {}

error_code invalid_regular_expression::code() const noexcept {
	return error_code::ts1507;
}

unterminated_regular_expression_literal::
	unterminated_regular_expression_literal(size_t offset) noexcept
	: regex_error(offset) {}

error_code unterminated_regular_expression_literal::code() const noexcept {
	return error_code::ts1509;
}

unterminated_character_class::unterminated_character_class(
	size_t offset) noexcept
	: regex_error(offset) {}

error_code unterminated_character_class::code() const noexcept {
	return error_code::ts1510;
}

invalid_escape_sequence::invalid_escape_sequence(size_t offset) noexcept
	: regex_error(offset) {}

error_code invalid_escape_sequence::code() const noexcept {
	return error_code::ts2301;
}

invalid_character_class_range::invalid_character_class_range(
	size_t offset) noexcept
	: regex_error(offset) {}

error_code invalid_character_class_range::code() const noexcept {
	return error_code::ts2413;
}

backreference_not_available::backreference_not_available(
	size_t offset,
	int backreference_number) noexcept
	: regex_error(offset), backreference_number_(backreference_number) {}

error_code backreference_not_available::code() const noexcept {
	return error_code::ts2414;
}

int backreference_not_available::backreference_number() const noexcept {
	return backreference_number_;
}

decimal_escape_in_character_class::decimal_escape_in_character_class(
	size_t offset) noexcept
	: regex_error(offset) {}

error_code decimal_escape_in_character_class::code() const noexcept {
	return error_code::ts2415;
}
