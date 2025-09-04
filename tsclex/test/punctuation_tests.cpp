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

TEST_CASE("Additional Punctuation and Edge Cases", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

	SECTION("Hash Symbol") {
		// Test # as standalone token (private fields, not shebang)
		auto tokens = tokenize("class { #privateField; }");
		REQUIRE(tokens.size() == 5);
		CHECK(tokens[0].is<tscc::lex::tokens::class_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::open_brace_token>());
		CHECK(tokens[2].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[2]->to_string() == "#privateField");
		CHECK(tokens[3].is<tscc::lex::tokens::semicolon_token>());
		CHECK(tokens[4].is<tscc::lex::tokens::close_brace_token>());
	}

	SECTION("Private Field Invalid Identifier") {
		// Test for invalid private field names
		auto lexer = create_lexer("class { #123; }");
		REQUIRE_THROWS(
			std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
	}

	SECTION("Underscore as Identifier") {
		auto tokens = tokenize("_ _unused __proto__");
		REQUIRE(tokens.size() == 3);
		CHECK(tokens[0].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[0]->to_string() == "_");
		CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[1]->to_string() == "_unused");
		CHECK(tokens[2].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[2]->to_string() == "__proto__");
	}
}