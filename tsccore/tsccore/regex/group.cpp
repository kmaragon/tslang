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

namespace tsccore::regex {

group::group(type group_type, disjunction disjunction, std::optional<std::string> name)
	: type_(group_type), disjunction_(std::move(disjunction)), name_(std::move(name)) {
}

group::group(const group& other)
	: type_(other.type_), disjunction_(other.disjunction_), name_(other.name_) {
}

group::group(group&& other) noexcept
	: type_(other.type_), disjunction_(std::move(other.disjunction_)), name_(std::move(other.name_)) {
}

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

const std::optional<std::string>& group::get_name() const {
	return name_;
}

}  // namespace tsccore::regex