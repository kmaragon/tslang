/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2026. Keef Aragon
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

#include "declare_in_ambient_context.hpp"

using namespace tscc;
using namespace tscc::parse;

declare_in_ambient_context::declare_in_ambient_context(
	const lex::source_location& location) noexcept
	: parse_error(location) {}

const char* declare_in_ambient_context::what() const noexcept {
	return "A 'declare' modifier cannot be used in an already ambient context.";
}

error_code declare_in_ambient_context::code() const noexcept {
	return error_code::ts1038;
}
