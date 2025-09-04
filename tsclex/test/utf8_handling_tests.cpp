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

TEST_CASE("Native UTF-8 Handling", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

	SECTION("Two-byte sequence") {
		auto tokens = tokenize(R"( const varµ = 12; )");
		REQUIRE(tokens.size() == 5);
		CHECK(tokens[0].is<tscc::lex::tokens::const_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[1]->to_string() == "varµ");
		CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::constant_value_token>());
		CHECK(tokens[3]->to_string() == "12");
		CHECK(tokens[4].is<tscc::lex::tokens::semicolon_token>());
	}

	SECTION("Three-byte sequence") {
		auto tokens = tokenize(R"( const varァ = 314.195e-2; )");
		REQUIRE(tokens.size() == 5);
		CHECK(tokens[0].is<tscc::lex::tokens::const_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[1]->to_string() == "varァ");
		CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::constant_value_token>());
		CHECK(tokens[3]->to_string().substr(0, 7) == "314.195");
		CHECK(tokens[3]->to_string().ends_with("e-2"));
		CHECK(tokens[4].is<tscc::lex::tokens::semicolon_token>());
	}

	SECTION("Four-byte sequence") {
		auto tokens =
			tokenize(R"( const unistr = "String with native 😀"; )");
		REQUIRE(tokens.size() == 5);
		CHECK(tokens[0].is<tscc::lex::tokens::const_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[1]->to_string() == "unistr");
		CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::constant_value_token>());
		CHECK(tokens[3]->to_string() ==
			  "\"String with native \\ud83d\\ude00\"");
		CHECK(tokens[4].is<tscc::lex::tokens::semicolon_token>());
	}

	SECTION("Invalid Unicode Escape Sequences") {
		// Test for invalid unicode escape sequences
		auto lexer =
			create_lexer("const x = '\\u{110000}';");  // Out of range
		REQUIRE_THROWS(
			std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
	}
}