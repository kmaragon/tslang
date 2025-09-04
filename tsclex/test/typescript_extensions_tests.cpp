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

TEST_CASE("TypeScript Extensions", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

	SECTION("Type Annotations") {
		auto tokens = tokenize("let x: string;");
		REQUIRE(tokens.size() == 5);
		CHECK(tokens[0].is<tscc::lex::tokens::let_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[1]->to_string() == "x");
		CHECK(tokens[2].is<tscc::lex::tokens::colon_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::string_token>());
		CHECK(tokens[4].is<tscc::lex::tokens::semicolon_token>());
	}

	SECTION("Generics") {
		auto tokens = tokenize("function<T>(arg: T): T");
		REQUIRE(tokens.size() == 11);
		CHECK(tokens[0].is<tscc::lex::tokens::function_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::less_token>());
		CHECK(tokens[2].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[2]->to_string() == "T");
		CHECK(tokens[3].is<tscc::lex::tokens::greater_token>());
		CHECK(tokens[4].is<tscc::lex::tokens::open_paren_token>());
		CHECK(tokens[5].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[5]->to_string() == "arg");
		CHECK(tokens[6].is<tscc::lex::tokens::colon_token>());
		CHECK(tokens[7].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[7]->to_string() == "T");
	}
}