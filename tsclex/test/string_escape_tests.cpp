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

TEST_CASE("String Escape Edge Cases", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

	SECTION("Additional Escape Sequences") {
		auto tokens = tokenize("'\\0\\b\\f\\v\\r'");
		REQUIRE(tokens.size() == 1);
		REQUIRE(tokens[0].is<tscc::lex::tokens::constant_value_token>());

		auto& cv = static_cast<tscc::lex::tokens::constant_value_token&>(
			*tokens[0]);
		auto sv = cv.string_value();
		REQUIRE(sv);
		REQUIRE(sv->at(0) == 0);
		REQUIRE(sv->at(1) == '\b');
		REQUIRE(sv->at(2) == '\f');
		REQUIRE(sv->at(3) == '\v');
		REQUIRE(sv->at(4) == '\r');
	}

	SECTION("String Without Terminating Newline") {
		auto tokens = tokenize("'hello'");
		REQUIRE(tokens.size() == 1);
		CHECK(tokens[0].is<tscc::lex::tokens::constant_value_token>());
		CHECK(tokens[0]->to_string() == "'hello'");
	}
}