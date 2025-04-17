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

#include <string>

namespace tscc {

/**
 * @brief Get the length of the jsonified string using the provided quote character
 *
 * This will consider instances of the quote character as needing to be escaped.
 * But the resulting size will not include any surrounding quotes
 */
std::size_t json_string_size(const std::u32string_view& str, char quote_char = '"');

/**
 * @brief Encode the string to a jsonified version of itself
 *
 * The resulting string will include the quotes and will have the quote char
 * escaped.
 */
std::string to_json_string(const std::u32string_view& str, char quote_char = '"');

}