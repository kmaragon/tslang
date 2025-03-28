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

#include "json.hpp"
#include <array>
#include <cassert>
#include "utf8.hpp"

std::size_t tscc::json_string_size(const std::u32string& str, char quote_char) {
	std::size_t len = quote_char ? 2 : 0;
	for (auto ch : str) {
		if (quote_char && ch == static_cast<char32_t>(quote_char)) {
			len += 2;
			continue;
		}

		if (ch >= 0xd800 && ch <= 0xdfff) {
			ch -= 0xd800;
		}

		// we only consider 0x00 - 0xff for normal utf-8 encoding
		// otherwise we \u escape
		if (ch <= 0x7f) {
			switch (ch) {
				case '\b':
				case '\f':
				case '\n':
				case '\r':
				case '\t':
					len += 1;
					[[fallthrough]];
				default:
					len += 1;
			}

			continue;
		}

		len += 2;
		if (ch <= 0xff) {
			// 2 byte simple utf-8 sequence
			continue;
		}

		len += 4;

		// single \uXXXX sequence
		if (ch <= 0xffff) {
			continue;
		}

		// otherwise, 2 consecutive uXXXX sequences
		len += 6;
	}

	return len;
}

std::string tscc::to_json_string(const std::u32string& str, char quote_char) {
	static constexpr std::array<char, 16> hex_chars{
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

	std::size_t size = json_string_size(str, quote_char);
	std::string result;
	result.resize(size);
	std::size_t wr = 0;

	if (quote_char) {
		result[0] = quote_char;
		wr = 1;
	}

	for (auto ch : str) {
		if (quote_char && ch == static_cast<char32_t>(quote_char)) {
			result[wr++] = '\\';
			result[wr++] = quote_char;
			continue;
		}

		if (ch >= 0xd800 && ch <= 0xdfff) {
			ch -= 0xd800;
		}

		if (ch <= 0x7f) {
			switch (ch) {
				case '\b':
					result[wr++] = '\\';
					result[wr++] = 'b';
					break;
				case '\f':
					result[wr++] = '\\';
					result[wr++] = 'f';
					break;
				case '\n':
					result[wr++] = '\\';
					result[wr++] = 'n';
					break;
				case '\r':
					result[wr++] = '\\';
					result[wr++] = 'r';
					break;
				case '\t':
					result[wr++] = '\\';
					result[wr++] = 't';
					break;
				default:
					result[wr++] = ch;
			}

			continue;
		}

		// utf-8 escape sequence
		if (ch <= 0xff) {
			result[wr++] = static_cast<char>(0xc0 | (ch >> 6));
			result[wr++] = static_cast<char>(0x80 | (ch & 0x3f));
			continue;
		}

		// \u escape sequences
		// single \uXXXX sequence
		if (ch <= 0xffff) {
			result[wr++] = '\\';
			result[wr++] = 'u';

			auto high = (ch >> 8) & 0xff;
			auto low = ch & 0xff;
			result[wr++] = hex_chars[high >> 4];
			result[wr++] = hex_chars[high & 0xf];

			result[wr++] = hex_chars[low >> 4];
			result[wr++] = hex_chars[low & 0xf];
		}

		ch -= 0x10000;
		auto high_utf16 = 0xd800 + ((ch >> 10) & 0x3ff);
		auto low_utf16 = 0xdc00 + (ch & 0x3ff);

		result[wr++] = '\\';
		result[wr++] = 'u';

		auto high_byte = (high_utf16 >> 8) & 0xff;
		auto low_byte = high_utf16 & 0xff;
		result[wr++] = hex_chars[high_byte >> 4];
		result[wr++] = hex_chars[high_byte & 0xf];

		result[wr++] = hex_chars[low_byte >> 4];
		result[wr++] = hex_chars[low_byte & 0xf];

		result[wr++] = '\\';
		result[wr++] = 'u';

		high_byte = (low_utf16 >> 8) & 0xff;
		low_byte = low_utf16 & 0xff;
		result[wr++] = hex_chars[high_byte >> 4];
		result[wr++] = hex_chars[high_byte & 0xf];

		result[wr++] = hex_chars[low_byte >> 4];
		result[wr++] = hex_chars[low_byte & 0xf];
	}

	assert(wr + (quote_char ? 1 : 0) == size);
	if (quote_char) {
		result[wr] = quote_char;
	}
	return result;
}