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

TEST_CASE("Lambdas", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

	SECTION("One parameter lambdas") {
		auto tokens = tokenize("let k = values.map(v => v.a);");
		REQUIRE(tokens.size() == 14);
		CHECK(tokens[0].is<tscc::lex::tokens::let_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[1]->to_string() == "k");
		CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[3]->to_string() == "values");
		CHECK(tokens[4].is<tscc::lex::tokens::dot_token>());
		CHECK(tokens[5].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[5]->to_string() == "map");
		CHECK(tokens[6].is<tscc::lex::tokens::open_paren_token>());
		CHECK(tokens[7].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[7]->to_string() == "v");
		CHECK(tokens[8].is<tscc::lex::tokens::eq_greater_token>());
		CHECK(tokens[9].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[9]->to_string() == "v");
		CHECK(tokens[10].is<tscc::lex::tokens::dot_token>());
		CHECK(tokens[11].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[11]->to_string() == "a");
		CHECK(tokens[12].is<tscc::lex::tokens::close_paren_token>());
		CHECK(tokens[13].is<tscc::lex::tokens::semicolon_token>());
	}

	SECTION("two parameter lambdas") {
		auto tokens = tokenize("let k = enjoin((v, b) => v.a + b);");
		REQUIRE(tokens.size() == 18);
		CHECK(tokens[0].is<tscc::lex::tokens::let_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[1]->to_string() == "k");
		CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[3]->to_string() == "enjoin");
		CHECK(tokens[4].is<tscc::lex::tokens::open_paren_token>());
		CHECK(tokens[5].is<tscc::lex::tokens::open_paren_token>());
		CHECK(tokens[6].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[6]->to_string() == "v");
		CHECK(tokens[7].is<tscc::lex::tokens::comma_token>());
		CHECK(tokens[8].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[8]->to_string() == "b");
		CHECK(tokens[9].is<tscc::lex::tokens::close_paren_token>());
		CHECK(tokens[10].is<tscc::lex::tokens::eq_greater_token>());
		CHECK(tokens[11].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[11]->to_string() == "v");
		CHECK(tokens[12].is<tscc::lex::tokens::dot_token>());
		CHECK(tokens[13].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[13]->to_string() == "a");
		CHECK(tokens[14].is<tscc::lex::tokens::plus_token>());
		CHECK(tokens[15].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[15]->to_string() == "b");
		CHECK(tokens[16].is<tscc::lex::tokens::close_paren_token>());
		CHECK(tokens[17].is<tscc::lex::tokens::semicolon_token>());
	}
}