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

#include <exception>
#include "source_location.hpp"

namespace tscc::lex
{

enum class error_code : std::uint16_t
{
	ts999 = 999,
	ts1002 = 1002,
	ts1003 = 1003,
	ts1010 = 1010,
	ts1125 = 1125,
	ts1126 = 1126,
	ts1127 = 1127,
	ts1198 = 1198,
	ts1199 = 1199,
	ts6188 = 6188,
	ts6189 = 6189,
	ts18026 = 18026,
};

/**
 * \brief A generic exception while lexing
 */
class lex_error : public std::exception {
public:
	lex_error(const source_location& location) noexcept;

	/**
	 * @brief Get the location where the error occurred
	 */
	const source_location& location() const noexcept;

	/**
	 * @brief Get the error code
	 */
	virtual error_code code() const noexcept = 0;

private:
	source_location location_;
};


}