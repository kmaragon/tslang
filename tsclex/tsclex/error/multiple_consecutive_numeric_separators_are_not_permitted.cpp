/*
* TSCC - a Typescript Compiler
* Copyright (c) 2023. Minh Lu
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

#include "multiple_consecutive_numeric_separators_are_not_permitted.hpp"

using namespace tscc::lex;

multiple_consecutive_numeric_separators_are_not_permitted::multiple_consecutive_numeric_separators_are_not_permitted(
	const tscc::lex::source_location& location) noexcept
	: lex_error(location) {}

const char* multiple_consecutive_numeric_separators_are_not_permitted::what() const noexcept {
	return "Multiple consecutive numeric separators are not permitted";
}

error_code multiple_consecutive_numeric_separators_are_not_permitted::code() const noexcept {
	return error_code::ts6189;
}