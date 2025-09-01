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

#include <optional>
#include <string>
#include <memory>
#include "disjunction.hpp"

namespace tsccore::regex {

class group {
public:
	enum class type {
		capturing,
		non_capturing,
		positive_lookahead,
		negative_lookahead,
		positive_lookbehind,
		negative_lookbehind
	};

	group(type group_type, disjunction disjunction, std::optional<std::u32string> name = std::nullopt);
	group(const group& other);
	group(group&& other) noexcept;
	group& operator=(const group& other);
	group& operator=(group&& other) noexcept;

	type get_type() const;
	const disjunction& get_disjunction() const;
	const std::optional<std::u32string>& get_name() const;

	std::size_t string_size() const noexcept;
	void to_string(std::u32string& to) const;

	bool operator==(const group& other) const noexcept;
	bool operator!=(const group& other) const noexcept;

private:
	type type_;
	disjunction disjunction_;
	std::optional<std::u32string> name_;
};

}  // namespace tsccore::regex