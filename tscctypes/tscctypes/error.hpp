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
#include <cstdint>

namespace tscc {

/**
 * \brief TypeScript diagnostic error codes
 *
 * These match the official TypeScript compiler diagnostic codes
 * for compatibility and familiarity.
 */
enum class error_code : std::uint16_t {
	// Lexer errors
	ts999 = 999,
	ts1002 = 1002,
	ts1003 = 1003,
	ts1010 = 1010,
	ts1125 = 1125,
	ts1126 = 1126,
	ts1127 = 1127,
	ts1198 = 1198,
	ts1199 = 1199,
	ts1499 = 1499,
	ts1500 = 1500,
	ts1501 = 1501,
	ts1502 = 1502,
	ts6188 = 6188,
	ts6189 = 6189,
	ts17008 = 17008,
	ts18026 = 18026,

	// Parser errors
	ts1005 = 1005,  // Expected token
	ts1109 = 1109,  // Expression expected
	ts1128 = 1128,  // Declaration or statement expected
	ts1146 = 1146,  // Declaration expected
	ts2304 = 2304,  // Cannot find name
};

/**
 * \brief Base exception class for all TSCC compiler errors
 *
 * All lexer, parser, and semantic errors inherit from this class.
 */
class tscc_exception : public std::exception {
public:
	virtual ~tscc_exception() = default;

	/**
	 * \brief Get the TypeScript-compatible error code
	 */
	virtual error_code code() const noexcept = 0;
};

}  // namespace tscc