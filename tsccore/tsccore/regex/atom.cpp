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

#include "atom.hpp"

using namespace tsccore::regex;

atom::atom(char32_t character) : value_(character) {}

atom::atom(builtin_class builtin_class) : value_(builtin_class) {}

atom::atom(character_class character_class)
	: value_(std::move(character_class)) {}

atom::atom(group group) : value_(std::move(group)) {}

bool atom::is_character() const {
	return std::holds_alternative<char32_t>(value_);
}

bool atom::is_builtin_class() const {
	return std::holds_alternative<builtin_class>(value_);
}

bool atom::is_character_class() const {
	return std::holds_alternative<character_class>(value_);
}

bool atom::is_group() const {
	return std::holds_alternative<group>(value_);
}

char32_t atom::get_character() const {
	return std::get<char32_t>(value_);
}

atom::builtin_class atom::get_builtin_class() const {
	return std::get<builtin_class>(value_);
}

const character_class& atom::get_character_class() const {
	return std::get<character_class>(value_);
}

const group& atom::get_group() const {
	return std::get<group>(value_);
}

std::size_t atom::string_size() const noexcept {
	if (is_character()) {
		char32_t ch = get_character();
		if (ch == U'.' || ch == U'*' || ch == U'+' || ch == U'?' ||
			ch == U'^' || ch == U'$' || ch == U'|' || ch == U'(' ||
			ch == U')' || ch == U'[' || ch == U']' || ch == U'{' ||
			ch == U'}' || ch == U'\\' || ch == U'\n' || ch == U'\r' ||
			ch == U'\t' || ch == U'\f' || ch == U'\v' || ch == U'\0' ||
			ch == U'/') {
			return 2;
		}
		return 1;
	} else if (is_builtin_class()) {
		return 2;
	} else if (is_character_class()) {
		return get_character_class().string_size();
	} else {
		return get_group().string_size();
	}
}

void atom::to_string(std::u32string& to) const {
	if (is_character()) {
		char32_t ch = get_character();
		if (ch == U'.' || ch == U'*' || ch == U'+' || ch == U'?' ||
			ch == U'^' || ch == U'$' || ch == U'|' || ch == U'(' ||
			ch == U')' || ch == U'[' || ch == U']' || ch == U'{' ||
			ch == U'}' || ch == U'\\' || ch == U'/') {
			to += U'\\';
			to += ch;
		} else if (ch == U'\n')
			to += U"\\n";
		else if (ch == U'\r')
			to += U"\\r";
		else if (ch == U'\t')
			to += U"\\t";
		else if (ch == U'\f')
			to += U"\\f";
		else if (ch == U'\v')
			to += U"\\v";
		else if (ch == U'\0')
			to += U"\\0";
		else
			to += ch;
	} else if (is_builtin_class()) {
		builtin_class bc = get_builtin_class();
		switch (bc) {
			case builtin_class::dot:
				to += U'.';
				break;
			case builtin_class::word:
				to += U"\\w";
				break;
			case builtin_class::non_word:
				to += U"\\W";
				break;
			case builtin_class::digit:
				to += U"\\d";
				break;
			case builtin_class::non_digit:
				to += U"\\D";
				break;
			case builtin_class::whitespace:
				to += U"\\s";
				break;
			case builtin_class::non_whitespace:
				to += U"\\S";
				break;
		}
	} else if (is_character_class()) {
		get_character_class().to_string(to);
	} else {
		get_group().to_string(to);
	}
}

bool atom::operator==(const atom& other) const noexcept {
	return value_ == other.value_;
}

bool atom::operator!=(const atom& other) const noexcept {
	return !(*this == other);
}
