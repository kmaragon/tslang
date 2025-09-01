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

#include "group.hpp"
#include "disjunction.hpp"

using namespace tsccore::regex;

group::group(type group_type,
			 disjunction disjunction,
			 std::optional<std::u32string> name)
	: type_(group_type),
	  disjunction_(std::move(disjunction)),
	  name_(std::move(name)) {}

group::group(const group& other)
	: type_(other.type_),
	  disjunction_(other.disjunction_),
	  name_(other.name_) {}

group::group(group&& other) noexcept
	: type_(other.type_),
	  disjunction_(std::move(other.disjunction_)),
	  name_(std::move(other.name_)) {}

group& group::operator=(const group& other) {
	if (this != &other) {
		type_ = other.type_;
		disjunction_ = other.disjunction_;
		name_ = other.name_;
	}
	return *this;
}

group& group::operator=(group&& other) noexcept {
	if (this != &other) {
		type_ = other.type_;
		disjunction_ = std::move(other.disjunction_);
		name_ = std::move(other.name_);
	}
	return *this;
}

group::type group::get_type() const {
	return type_;
}

const disjunction& group::get_disjunction() const {
	return disjunction_;
}

const std::optional<std::u32string>& group::get_name() const {
	return name_;
}

std::size_t group::string_size() const noexcept {
	std::size_t baseline = 2;  // ()
	if (name_) {
		baseline += 3 + name_->size();	// ?<{name}>
	} else {
		switch (type_) {
			case type::non_capturing:		// ?:
			case type::positive_lookahead:	// ?=
			case type::negative_lookahead:	// ?!
				baseline += 2;
				break;
			case type::negative_lookbehind:	 // ?<=
			case type::positive_lookbehind:	 // ?<!
				baseline += 3;
				break;
			default:
				baseline += 10;
		}
	}

	return baseline + disjunction_.string_size();
}

void group::to_string(std::u32string& to) const {
	to += U'(';
	if (name_) {
		to += U"?<";
		to += *name_;
		to += U'>';
	} else {
		switch (type_) {
			case type::non_capturing:
				to += U"?:";
				break;
			case type::positive_lookahead:
				to += U"?=";
				break;
			case type::negative_lookahead:
				to += U"?!";
				break;
			case type::negative_lookbehind:	 // ?<=
				to += U"?<=";
				break;
			case type::positive_lookbehind:	 // ?<!
				to += U"?<!";
				break;
			default:
				break;
		}
	}

	disjunction_.to_string(to);
	to += U')';
}

bool group::operator==(const group& other) const noexcept {
	return type_ == other.type_ && disjunction_ == other.disjunction_ &&
		   name_ == other.name_;
}

bool group::operator!=(const group& other) const noexcept {
	return !(*this == other);
}
