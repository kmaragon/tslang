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

#pragma once

#include <variant>
#include <string>
#include <cstdint>
#include "character_class.hpp"
#include "group.hpp"

namespace tsccore::regex {

class atom {
public:
	enum class builtin_class {
		dot,           // .
		word,          // \w
		non_word,      // \W
		digit,         // \d
		non_digit,     // \D
		whitespace,    // \s
		non_whitespace // \S
	};

	atom(char32_t character);
	atom(builtin_class builtin_class);
	atom(character_class character_class);
	atom(group group);
	
	bool is_character() const;
	bool is_builtin_class() const;
	bool is_character_class() const;
	bool is_group() const;
	
	char32_t get_character() const;
	builtin_class get_builtin_class() const;
	const character_class& get_character_class() const;
	const group& get_group() const;

private:
	std::variant<char32_t, builtin_class, character_class, group> value_;
};

}  // namespace tsccore::regex
