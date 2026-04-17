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

#include "export_default_not_at_top_level.hpp"

using namespace tscc;
using namespace tscc::parse;

export_default_not_at_top_level::export_default_not_at_top_level(
	const lex::source_location& location) noexcept
	: parse_error(location) {}

const char* export_default_not_at_top_level::what() const noexcept {
	return "A default export must be at the top level of a file or module "
		   "declaration.";
}

error_code export_default_not_at_top_level::code() const noexcept {
	return error_code::ts1258;
}
