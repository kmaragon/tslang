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

#include "regular_expression.hpp"

using namespace tsccore::regex;

regular_expression::regular_expression(disjunction disjunction)
	: disjunction_(std::move(disjunction)) {}

const disjunction& regular_expression::get_disjunction() const {
	return disjunction_;
}

void regular_expression::set_disjunction(disjunction disjunction) {
	disjunction_ = std::move(disjunction);
}

std::size_t regular_expression::string_size() const noexcept {
	return disjunction_.string_size();
}

void regular_expression::to_string(std::u32string& to) const {
	disjunction_.to_string(to);
}

bool regular_expression::operator==(
	const regular_expression& other) const noexcept {
	return disjunction_ == other.disjunction_;
}

bool regular_expression::operator!=(
	const regular_expression& other) const noexcept {
	return !(*this == other);
}
