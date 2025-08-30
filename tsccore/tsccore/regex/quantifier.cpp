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

namespace tsccore::regex {

quantifier::quantifier(prefix prefix_type)
	: value_(prefix_type) {
}

quantifier::quantifier(std::pair<std::size_t, std::size_t> min_max)
	: value_(min_max) {
}

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

}  // namespace tsccore::regex
