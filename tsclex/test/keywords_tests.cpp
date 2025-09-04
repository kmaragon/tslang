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

TEST_CASE("Known Keywords", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

	SECTION("Class Declaration") {
		auto tokens = tokenize(R"(
class MyClass extends MyBase implements IMyInterface
{
    constructor()
    {
    }

	interfaceMethod(a: string, b: number): void
    {
    }
}
)");
		REQUIRE(tokens.size() == 37);
		CHECK(tokens[0].is<tscc::lex::tokens::newline_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::class_token>());
		CHECK(tokens[2].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[2]->to_string() == "MyClass");
		CHECK(tokens[3].is<tscc::lex::tokens::extends_token>());
		CHECK(tokens[4].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[4]->to_string() == "MyBase");
		CHECK(tokens[5].is<tscc::lex::tokens::implements_token>());
		CHECK(tokens[6].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[6]->to_string() == "IMyInterface");
		CHECK(tokens[7].is<tscc::lex::tokens::newline_token>());
		CHECK(tokens[8].is<tscc::lex::tokens::open_brace_token>());
		CHECK(tokens[9].is<tscc::lex::tokens::newline_token>());
		CHECK(tokens[10].is<tscc::lex::tokens::constructor_token>());
		CHECK(tokens[11].is<tscc::lex::tokens::open_paren_token>());
		CHECK(tokens[12].is<tscc::lex::tokens::close_paren_token>());
		CHECK(tokens[13].is<tscc::lex::tokens::newline_token>());
		CHECK(tokens[14].is<tscc::lex::tokens::open_brace_token>());
		CHECK(tokens[15].is<tscc::lex::tokens::newline_token>());
		CHECK(tokens[16].is<tscc::lex::tokens::close_brace_token>());
		CHECK(tokens[17].is<tscc::lex::tokens::newline_token>());
		CHECK(tokens[18].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[18]->to_string() == "interfaceMethod");
		CHECK(tokens[19].is<tscc::lex::tokens::open_paren_token>());
		CHECK(tokens[20].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[20]->to_string() == "a");
		CHECK(tokens[21].is<tscc::lex::tokens::colon_token>());
		CHECK(tokens[22].is<tscc::lex::tokens::string_token>());
		CHECK(tokens[23].is<tscc::lex::tokens::comma_token>());
		CHECK(tokens[24].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[24]->to_string() == "b");
		CHECK(tokens[25].is<tscc::lex::tokens::colon_token>());
		CHECK(tokens[26].is<tscc::lex::tokens::number_token>());
		CHECK(tokens[27].is<tscc::lex::tokens::close_paren_token>());
		CHECK(tokens[28].is<tscc::lex::tokens::colon_token>());
		CHECK(tokens[29].is<tscc::lex::tokens::void_token>());
		CHECK(tokens[30].is<tscc::lex::tokens::newline_token>());
		CHECK(tokens[31].is<tscc::lex::tokens::open_brace_token>());
		CHECK(tokens[32].is<tscc::lex::tokens::newline_token>());
		CHECK(tokens[33].is<tscc::lex::tokens::close_brace_token>());
		CHECK(tokens[34].is<tscc::lex::tokens::newline_token>());
		CHECK(tokens[35].is<tscc::lex::tokens::close_brace_token>());
		CHECK(tokens[36].is<tscc::lex::tokens::newline_token>());
	}
}