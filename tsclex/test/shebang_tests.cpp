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

#include "test_common.hpp"

TEST_CASE("Shebang", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

	SECTION("Single line shebang") {
		auto tokens = tokenize("#!  /bin/bash");
		REQUIRE(tokens.size() == 1);
		CHECK(tokens[0]->to_string() == "#!/bin/bash");
	}

	SECTION("Single line shebang with spaces and newline") {
		auto tokens = tokenize("#!  /bin/bash   \n");
		REQUIRE(tokens.size() == 1);
		CHECK(tokens[0]->to_string() == "#!/bin/bash");
	}

	SECTION("Shebang not at start of file") {
		auto lexer = create_lexer("const x = 1;\n#! /bin/bash");
		REQUIRE_THROWS(
			std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
	}
}