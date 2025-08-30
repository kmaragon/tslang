/**
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
#include <utility>
#include <variant>

namespace tsccore::regex {

class quantifier {
public:
	enum class prefix : char {
		zero_or_more = '*',
		one_or_more = '+',
		zero_or_one = '?'
	};

	explicit quantifier(prefix prefix_type);
	explicit quantifier(std::pair<std::size_t, std::size_t> min_max);
	
	bool is_prefix() const;
	bool is_range() const;
	
	prefix get_prefix() const;
	const std::pair<std::size_t, std::size_t>& get_range() const;

private:
	using min_max_length = std::pair<std::size_t, std::size_t>;
	std::variant<prefix, min_max_length> value_;
};

}  // namespace tsccore::regex
