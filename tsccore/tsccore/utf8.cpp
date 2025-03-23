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

#include "utf8.hpp"

std::size_t tscc::utf8_size(const std::u32string& str) noexcept {
	std::size_t len = str.size();
	for (std::size_t i = 0; i < str.size(); i++) {
		if (str[i] <= 0x7f)
			continue;

		len += 1;
		if (str[i] <= 0x7ff)
			continue;

		len += 1;
		if (str[i] <= 0xffff)
			continue;

		len += 1;
		if (str[i] <= 0x1fffff)
			continue;

		len += 1;
		if (str[i] <= 0x3ffffff)
			continue;

		len += 1;
	}

	return len;
}

std::string tscc::utf8_encode(const std::u32string& str) noexcept
{
	std::string result;
	result.resize(utf8_size(str));

	std::size_t wr = 0;
	for (auto& ch : str) {
		if (ch <= 0x7f) {
			result[wr++] = static_cast<char>(ch);
			continue;
		}

		if (ch <= 0x7ff) {
			result[wr++] = static_cast<char>(0xc0 | (ch >> 6));
			result[wr++] = static_cast<char>(0x80 | (ch & 0x3f));
			continue;
		}

		if (ch <= 0xffff) {
			result[wr++] = static_cast<char>(0xe0 | (ch >> 12));
			result[wr++] = static_cast<char>(0x80 | ((ch >> 6) & 0x3f));
			result[wr++] = static_cast<char>(0x80 | (ch & 0x3f));
			continue;
		}

		if (ch <= 0x1fffff) {
			result[wr++] = static_cast<char>(0xf0 | (ch >> 18));
			result[wr++] = static_cast<char>(0x80 | ((ch >> 12) & 0x3f));
			result[wr++] = static_cast<char>(0x80 | ((ch >> 6) & 0x3f));
			result[wr++] = static_cast<char>(0x80 | (ch & 0x3f));
			continue;
		}

		if (ch <= 0x3ffffff) {
			result[wr++] = static_cast<char>(0xf8 | (ch >> 24));
			result[wr++] = static_cast<char>(0x80 | ((ch >> 18) & 0x3f));
			result[wr++] = static_cast<char>(0x80 | ((ch >> 12) & 0x3f));
			result[wr++] = static_cast<char>(0x80 | ((ch >> 6) & 0x3f));
			result[wr++] = static_cast<char>(0x80 | (ch & 0x3f));
			continue;
		}

		result[wr++] = static_cast<char>(0xfc | (ch >> 30));
		result[wr++] = static_cast<char>(0x80 | ((ch >> 24) & 0x3f));
		result[wr++] = static_cast<char>(0x80 | ((ch >> 18) & 0x3f));
		result[wr++] = static_cast<char>(0x80 | ((ch >> 12) & 0x3f));
		result[wr++] = static_cast<char>(0x80 | ((ch >> 6) & 0x3f));
		result[wr++] = static_cast<char>(0x80 | (ch & 0x3f));
	}

	return result;
}