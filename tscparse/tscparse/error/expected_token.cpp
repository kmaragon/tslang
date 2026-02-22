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

#include "expected_token.hpp"
#include <format>

using namespace tscc;
using namespace tscc::parse;

expected_token::expected_token(const lex::source_location& location,
							   const std::string_view& expected,
							   const std::string_view& found) noexcept
	: parse_error(location),
	  expected_(expected),
	  found_(found) {}

const char* expected_token::what() const noexcept {
	if (message_.empty()) {
		message_ =
			std::format("Expected '{}' but found '{}'", expected_, found_);
	}
	return message_.c_str();
}

error_code expected_token::code() const noexcept {
	return error_code::ts1005;
}

const std::string& expected_token::expected() const noexcept {
	return expected_;
}

const std::string& expected_token::found() const noexcept {
	return found_;
}