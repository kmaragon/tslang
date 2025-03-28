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

#include <string>
#include "basic_token.hpp"

namespace tscc::lex::tokens {

/**
 * @brief A typescript token that represents a comment
 */
class conflict_marker_trivia_token : public basic_token {
public:
	conflict_marker_trivia_token(char prefix_char, const std::u32string& comment);

	bool operator==(const conflict_marker_trivia_token& other) const;
	bool operator!=(const conflict_marker_trivia_token& other) const;

	std::string to_string() const override;

private:
	std::u32string body_;
	char prefix_;
};

}  // namespace tscc::lex::tokens