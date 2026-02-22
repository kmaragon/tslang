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

#include "qualified_name.hpp"

using namespace tscc::parse::ast;

bool qualified_name::empty() const noexcept {
	return segments_.empty();
}

size_t qualified_name::size() const noexcept {
	return segments_.size();
}

qualified_name::operator bool() const noexcept {
	return !segments_.empty();
}

lexeme<std::string_view> qualified_name::operator[](size_t i) const {
	return {&segments_[i]};
}

const std::vector<tscc::lex::token>& qualified_name::tokens() const noexcept {
	return segments_;
}
