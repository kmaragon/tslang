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

#include "source_location.hpp"

using namespace tscc::lex;

source_location::source_location(std::shared_ptr<source> source,
                                 std::size_t line,
                                 std::size_t column,
                                 std::size_t offset) noexcept
	: source_(std::move(source)),
	  line_(line),
	  column_(column),
	  offset_(offset) {
}

source_location source_location::operator+(std::size_t offset) const noexcept {
	auto result = *this;
	result.offset_ += offset;
	return result;
}
