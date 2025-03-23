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

#include <span>
#include <string>
#include <vector>
#include "basic_token.hpp"

namespace tscc::lex::tokens {

/**
 * @brief A typescript token that represents a comment
 */
class multiline_comment_token : public basic_token {
public:
	multiline_comment_token(const std::span<std::u32string>& comment_lines);

	bool operator==(const multiline_comment_token& other) const;
	bool operator!=(const multiline_comment_token& other) const;

	std::string to_string() const override;

private:
	std::vector<std::string> lines_;
};

}  // namespace tscc::lex::tokens