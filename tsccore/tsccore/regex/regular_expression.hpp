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
#include "disjunction.hpp"

namespace tsccore::regex {
class regular_expression {
public:
	regular_expression() = default;
	explicit regular_expression(disjunction disjunction);

	const disjunction& get_disjunction() const;
	void set_disjunction(disjunction disjunction);

	std::size_t string_size() const noexcept;
	void to_string(std::u32string& to) const;

	bool operator==(const regular_expression& other) const noexcept;
	bool operator!=(const regular_expression& other) const noexcept;

private:
	disjunction disjunction_;
};
}  // namespace tsccore::regex