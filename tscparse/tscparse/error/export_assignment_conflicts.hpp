/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2026. Keef Aragon
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

#include "../error.hpp"

namespace tscc::parse {

/**
 * \brief Error when `export =` coexists with other exports
 *
 * Corresponds to TypeScript error TS2309:
 * "An export assignment cannot be used in a module with other exported
 * elements."
 */
class export_assignment_conflicts : public parse_error {
public:
	explicit export_assignment_conflicts(
		const lex::source_location& location) noexcept;

	const char* what() const noexcept override;

	error_code code() const noexcept override;
};

}  // namespace tscc::parse
