/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2022. Keef Aragon
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

#include <catch2/catch_test_macros.hpp>
#include <tsclex/lexer.hpp>
#include <tsclex/token.hpp>
#include "fake_source.hpp"

TEST_CASE("Lexer", "[lexer]") {
	SECTION("Shebang") {
		SECTION("Single line shebang") {
			auto source = std::make_shared<fake_source>(__FILE_NAME__);
			std::stringstream file{"#!  /bin/bash"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 2);
			REQUIRE(tokens[0]->to_string() == "#!");
			REQUIRE(tokens[1]->to_string() == "/bin/bash");
		}

		SECTION("Single line shebang with spaces and newline") {
			auto source = std::make_shared<fake_source>(__FILE_NAME__);
			std::stringstream file{"#!  /bin/bash   \n"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 2);
			REQUIRE(tokens[0]->to_string() == "#!");
			REQUIRE(tokens[1]->to_string() == "/bin/bash");
		}
	}
}