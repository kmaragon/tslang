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

#include "character_class.hpp"

using namespace tsccore::regex;

character_class::character_class(bool negated) : negated_(negated) {}

void character_class::add_character(char32_t character) {
	characters_.insert(character);
}

void character_class::add_range(char32_t start, char32_t end) {
	ranges_.emplace_back(start, end);
}

void character_class::add_range(range range) {
	ranges_.push_back(range);
}

bool character_class::is_negated() const {
	return negated_;
}

const std::set<char32_t>& character_class::get_characters() const {
	return characters_;
}

const std::vector<character_class::range>& character_class::get_ranges() const {
	return ranges_;
}

std::size_t character_class::string_size() const noexcept {
	std::size_t size = 1;
	if (negated_)
		size += 1;

	for (char32_t ch : characters_) {
		if (ch == U']' || ch == U'\\' || ch == U'-') {
			size += 2;
		} else {
			size += 1;
		}
	}

	for (const auto& range : ranges_) {
		if (range.first == U']' || range.first == U'\\' ||
			range.first == U'-') {
			size += 2;
		} else {
			size += 1;
		}
		size += 1;
		if (range.second == U']' || range.second == U'\\' ||
			range.second == U'-') {
			size += 2;
		} else {
			size += 1;
		}
	}

	size += 1;
	return size;
}

void character_class::to_string(std::u32string& to) const {
	to += U'[';
	if (negated_) {
		to += U'^';
	}

	for (char32_t ch : characters_) {
		if (ch == U']' || ch == U'\\' || ch == U'-') {
			to += U'\\';
		}
		to += ch;
	}

	for (const auto& range : ranges_) {
		if (range.first == U']' || range.first == U'\\' ||
			range.first == U'-') {
			to += U'\\';
		}
		to += range.first;
		to += U'-';
		if (range.second == U']' || range.second == U'\\' ||
			range.second == U'-') {
			to += U'\\';
		}
		to += range.second;
	}

	to += U']';
}

bool character_class::operator==(const character_class& other) const noexcept {
	return negated_ == other.negated_ && characters_ == other.characters_ &&
		   ranges_ == other.ranges_;
}

bool character_class::operator!=(const character_class& other) const noexcept {
	return !(*this == other);
}
