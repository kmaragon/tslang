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
#include <algorithm>
#include <span>
#include <vector>

namespace tsccore::regex {

class alternative;

class disjunction {
public:
	disjunction();
	template <typename BeginIterator, typename EndIterator>
	disjunction(BeginIterator begin, const EndIterator& end) {
		for (; begin != end; ++begin) {
			add_alternative(std::move(*begin));
		}
	}
	disjunction(const disjunction& other);
	disjunction(disjunction&& other) noexcept;
	~disjunction();

	disjunction& operator=(const disjunction& other);
	disjunction& operator=(disjunction&& other) noexcept;

	[[nodiscard]] std::span<const alternative> get_alternatives() const;
	void add_alternative(alternative&& alternative);

private:
	void reset() noexcept;

	alignas(std::max_align_t)
		std::byte storage_[sizeof(std::vector<std::max_align_t>) +
						   sizeof(std::max_align_t)];
	int index_;
};
}  // namespace tsccore::regex