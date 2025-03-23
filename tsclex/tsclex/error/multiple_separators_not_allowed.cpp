// TSCC - a Typescript Compiler
// Copyright (c) 2025. Keef Aragon
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#include "multiple_separators_not_allowed.hpp"

using namespace tscc::lex;

multiple_separators_not_allowed::multiple_separators_not_allowed(
	const tscc::lex::source_location& location) noexcept
	: lex_error(location) {}

const char* multiple_separators_not_allowed::what() const noexcept {
	return "Multiple consecutive numeric separators are not permitted.";
}

error_code multiple_separators_not_allowed::code() const noexcept {
	return error_code::ts6189;
}