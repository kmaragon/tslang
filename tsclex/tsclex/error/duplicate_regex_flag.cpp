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

#include "duplicate_regex_flag.hpp"

using namespace tscc::lex;

duplicate_regex_flag::duplicate_regex_flag(
	const tscc::lex::source_location& location) noexcept
	: lex_error(location) {}

const char* duplicate_regex_flag::what() const noexcept {
	return "Duplicate regular expression flag.";
}

error_code duplicate_regex_flag::code() const noexcept {
	return error_code::ts1500;
}