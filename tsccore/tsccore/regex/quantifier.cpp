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

#include "quantifier.hpp"
#include <limits>

using namespace tsccore::regex;

namespace {
constexpr std::size_t count_digits(std::size_t value) noexcept {
	if (value == 0)
		return 1;

	std::size_t digits = 0;
	std::size_t threshold = 1;
	while (value >= threshold) {
		digits++;
		if (threshold > std::numeric_limits<std::size_t>::max() / 10)
			break;
		threshold *= 10;
	}
	return digits;
}

void append_number(std::u32string& to, std::size_t value) {
	if (value == 0) {
		to += U'0';
		return;
	}

	std::size_t digits = count_digits(value);
	std::size_t start_pos = to.size();
	to.resize(to.size() + digits);

	for (std::size_t i = digits; i > 0; --i) {
		to[start_pos + i - 1] = U'0' + (value % 10);
		value /= 10;
	}
}

}  // namespace

quantifier::quantifier(prefix prefix_type) : value_(prefix_type) {}

quantifier::quantifier(std::pair<std::size_t, std::size_t> min_max)
	: value_(min_max) {}

bool quantifier::is_prefix() const {
	return std::holds_alternative<prefix>(value_);
}

bool quantifier::is_range() const {
	return std::holds_alternative<min_max_length>(value_);
}

quantifier::prefix quantifier::get_prefix() const {
	return std::get<prefix>(value_);
}

const std::pair<std::size_t, std::size_t>& quantifier::get_range() const {
	return std::get<min_max_length>(value_);
}

std::size_t quantifier::string_size() const noexcept {
	if (is_prefix()) {
		return 1;
	} else {
		const auto& range = get_range();
		std::size_t digits_min = count_digits(range.first);

		if (range.second == std::numeric_limits<std::size_t>::max()) {
			return 1 + digits_min + 2;
		} else if (range.first == range.second) {
			return 1 + digits_min + 1;
		} else {
			std::size_t digits_max = count_digits(range.second);
			return 1 + digits_min + 1 + digits_max + 1;
		}
	}
}

void quantifier::to_string(std::u32string& to) const {
	if (is_prefix()) {
		to += static_cast<char32_t>(get_prefix());
	} else {
		const auto& range = get_range();
		to += U'{';

		append_number(to, range.first);

		if (range.second == std::numeric_limits<std::size_t>::max()) {
			to += U',';
		} else if (range.first != range.second) {
			to += U',';
			append_number(to, range.second);
		}
		to += U'}';
	}
}

bool quantifier::operator==(const quantifier& other) const noexcept {
	return value_ == other.value_;
}

bool quantifier::operator!=(const quantifier& other) const noexcept {
	return !(*this == other);
}