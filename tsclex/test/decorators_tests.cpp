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

TEST_CASE("Decorators", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

	SECTION("Class Decorator") {
		auto tokens = tokenize("@decorator class MyClass {}");
		REQUIRE(tokens.size() == 6);
		CHECK(tokens[0].is<tscc::lex::tokens::at_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[1]->to_string() == "decorator");
		CHECK(tokens[2].is<tscc::lex::tokens::class_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[3]->to_string() == "MyClass");
		CHECK(tokens[4].is<tscc::lex::tokens::open_brace_token>());
		CHECK(tokens[5].is<tscc::lex::tokens::close_brace_token>());
	}

	SECTION("Method Decorator") {
		auto tokens = tokenize("@readonly get value() {}");
		REQUIRE(tokens.size() == 8);
		CHECK(tokens[0].is<tscc::lex::tokens::at_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[1]->to_string() == "readonly");
		CHECK(tokens[2].is<tscc::lex::tokens::get_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[3]->to_string() == "value");
		CHECK(tokens[4].is<tscc::lex::tokens::open_paren_token>());
		CHECK(tokens[5].is<tscc::lex::tokens::close_paren_token>());
		CHECK(tokens[6].is<tscc::lex::tokens::open_brace_token>());
		CHECK(tokens[7].is<tscc::lex::tokens::close_brace_token>());
	}
}