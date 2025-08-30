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

#pragma once

#include <exception>
#include <cinttypes>

namespace tsccore::regex
{

enum class error_code : std::uint16_t
{
	ts1507 = 1507,  // Invalid regular expression
	ts1509 = 1509,  // Unterminated regular expression literal
	ts1510 = 1510,  // Unterminated character class in regular expression
	ts2301 = 2301,  // Invalid escape sequence in regular expression
	ts2413 = 2413,  // Invalid range in character class
	ts2414 = 2414,  // Backreference '\{0}' is not available
	ts2415 = 2415,  // Decimal escape sequences and backreferences are not allowed in character classes
};

/**
 * \brief A generic exception while lexing
 */
class regex_error : public std::exception {
public:
	regex_error(size_t offset) noexcept;

	/**
	 * \brief Get the location where the error occurred
	 */
	size_t offset() const noexcept;

	/**
	 * \brief Get the error code
	 */
	virtual error_code code() const noexcept = 0;

private:
	size_t offset_;
};

class invalid_regular_expression : public regex_error {
public:
	invalid_regular_expression(size_t offset) noexcept;
	error_code code() const noexcept override;
};

class unterminated_regular_expression_literal : public regex_error {
public:
	unterminated_regular_expression_literal(size_t offset) noexcept;
	error_code code() const noexcept override;
};

class unterminated_character_class : public regex_error {
public:
	unterminated_character_class(size_t offset) noexcept;
	error_code code() const noexcept override;
};

class invalid_escape_sequence : public regex_error {
public:
	invalid_escape_sequence(size_t offset) noexcept;
	error_code code() const noexcept override;
};

class invalid_character_class_range : public regex_error {
public:
	invalid_character_class_range(size_t offset) noexcept;
	error_code code() const noexcept override;
};

class backreference_not_available : public regex_error {
public:
	backreference_not_available(size_t offset, int backreference_number) noexcept;
	error_code code() const noexcept override;
	int backreference_number() const noexcept;

private:
	int backreference_number_;
};

class decimal_escape_in_character_class : public regex_error {
public:
	decimal_escape_in_character_class(size_t offset) noexcept;
	error_code code() const noexcept override;
};

}