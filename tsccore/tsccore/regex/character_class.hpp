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
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace tsccore::regex {

class character_class {
public:
	using range = std::pair<char32_t, char32_t>;

	character_class(bool negated = false);

	void add_character(char32_t character);
	void add_range(char32_t start, char32_t end);
	void add_range(range range);

	bool is_negated() const;
	const std::set<char32_t>& get_characters() const;
	const std::vector<range>& get_ranges() const;

	std::size_t string_size() const noexcept;
	void to_string(std::u32string& to) const;

	bool operator==(const character_class& other) const noexcept;
	bool operator!=(const character_class& other) const noexcept;

private:
	bool negated_;
	std::set<char32_t> characters_;
	std::vector<range> ranges_;
};

}  // namespace tsccore::regex