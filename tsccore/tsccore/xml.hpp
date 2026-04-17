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
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>

namespace tscc {

/**
 * \brief Policy for handling characters outside the XML Char production
 *
 * XML 1.0 restricts legal characters to:
 *   #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]
 *
 * This enum controls what happens when encode encounters a codepoint
 * outside that range, or when decode encounters a numeric character
 * reference to such a codepoint.
 */
enum class xml_invalid_char {
	throw_exception,
	drop,
};

/**
 * \brief Thrown when an invalid XML character is encountered
 */
class invalid_xml_character : public std::runtime_error {
public:
	invalid_xml_character(char32_t codepoint, std::size_t position);

	char32_t codepoint() const noexcept { return codepoint_; }
	std::size_t position() const noexcept { return position_; }

private:
	char32_t codepoint_;
	std::size_t position_;
};

std::u32string xml_decode(
	const std::u32string_view& input,
	xml_invalid_char policy = xml_invalid_char::throw_exception);

std::u32string xml_encode(
	const std::u32string_view& input,
	xml_invalid_char policy = xml_invalid_char::throw_exception);

}