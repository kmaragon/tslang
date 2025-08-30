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

#include "unicode_value_out_of_range.hpp"

using namespace tscc::lex;

unicode_value_out_of_range::unicode_value_out_of_range(
	const tscc::lex::source_location& location) noexcept
	: lex_error(location) {}

const char* unicode_value_out_of_range::what() const noexcept {
	return "An extended Unicode escape value must be between 0x0 and 0x10FFFF "
		   "inclusive.";
}

error_code unicode_value_out_of_range::code() const noexcept {
	return error_code::ts1198;
}