/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2022. Keef Aragon
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

#include "premature_end_of_file.hpp"

using namespace tscc::lex;

premature_end_of_file::premature_end_of_file(
	const tscc::lex::source_location& location) noexcept
	: lex_error(location) {}

const char* premature_end_of_file::what() const noexcept {
	return "Premature end of file while processing the token starting at the "
		   "given location";
}
