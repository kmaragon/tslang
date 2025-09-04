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

#include "conflicting_regex_flags.hpp"

using namespace tscc::lex;

conflicting_regex_flags::conflicting_regex_flags(
	const tscc::lex::source_location& location) noexcept
	: lex_error(location) {}

const char* conflicting_regex_flags::what() const noexcept {
	return "The 'u' and 'v' flags cannot be specified together.";
}

error_code conflicting_regex_flags::code() const noexcept {
	return error_code::ts1502;
}