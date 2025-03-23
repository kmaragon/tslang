// TSCC - a Typescript Compiler
// Copyright (c) 2025. Keef Aragon
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "../error.hpp"

namespace tscc::lex {
	class unicode_value_out_of_range : public lex_error {
	public:
		unicode_value_out_of_range(const source_location &location) noexcept;

		const char *what() const noexcept override;

		error_code code() const noexcept override;
	};
}
