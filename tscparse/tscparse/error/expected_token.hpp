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
#include "../error.hpp"

namespace tscc::parse {

/**
 * \brief Expected a specific token but found something else
 */
class expected_token : public parse_error {
public:
	expected_token(const lex::source_location& location,
				   std::string expected,
				   std::string found) noexcept;

	const char* what() const noexcept override;

	error_code code() const noexcept override;

	const std::string& expected() const noexcept;
	const std::string& found() const noexcept;

private:
	std::string expected_;
	std::string found_;
	mutable std::string message_;
};

}  // namespace tscc::parse