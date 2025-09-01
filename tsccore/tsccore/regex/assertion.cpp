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

#include "assertion.hpp"

using namespace tsccore::regex;

assertion::assertion(type assertion_type) : type_(assertion_type) {}

assertion::type assertion::get_type() const {
	return type_;
}

std::size_t assertion::string_size() const noexcept {
	switch (type_) {
		case type::start_of_line:
		case type::end_of_line:
			return 1;
		case type::word_boundary:
		case type::non_word_boundary:
			return 2;
		default:
			return 0;
	}
}

void assertion::to_string(std::u32string& to) const {
	switch (type_) {
		case type::start_of_line:
			to += U'^';
			break;
		case type::end_of_line:
			to += U'$';
			break;
		case type::word_boundary:
			to += U"\\b";
			break;
		case type::non_word_boundary:
			to += U"\\B";
			break;
	}
}

bool assertion::operator==(const assertion& other) const noexcept {
	return type_ == other.type_;
}

bool assertion::operator!=(const assertion& other) const noexcept {
	return !(*this == other);
}
