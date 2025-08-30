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

namespace tsccore::regex {

atom::atom(char32_t character)
	: value_(character) {
}

atom::atom(builtin_class builtin_class)
	: value_(builtin_class) {
}

atom::atom(character_class character_class)
	: value_(std::move(character_class)) {
}

atom::atom(group group)
	: value_(std::move(group)) {
}

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

}  // namespace tsccore::regex
