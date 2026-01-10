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

#include <tscctypes/error.hpp>
#include "source_location.hpp"

namespace tscc::lex
{

/**
 * \brief A generic exception while lexing
 */
class lex_error : public tscc_exception {
public:
	lex_error(const source_location& location) noexcept;

	/**
	 * \brief Get the location where the error occurred
	 */
	const source_location& location() const noexcept;

	/**
	 * \brief Get the error code
	 */
	virtual error_code code() const noexcept = 0;

private:
	source_location location_;
};


}