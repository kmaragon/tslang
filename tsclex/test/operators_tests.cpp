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

TEST_CASE("Operators", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

	SECTION("Arithmetic Operators") {
		auto tokens = tokenize("+ - * / % ** ++ -- & ^ ~");
		REQUIRE(tokens.size() == 11);
		CHECK(tokens[0].is<tscc::lex::tokens::plus_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::minus_token>());
		CHECK(tokens[2].is<tscc::lex::tokens::asterisk_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::slash_token>());
		CHECK(tokens[4].is<tscc::lex::tokens::percent_token>());
		CHECK(tokens[5].is<tscc::lex::tokens::double_asterisk_token>());
		CHECK(tokens[6].is<tscc::lex::tokens::double_plus_token>());
		CHECK(tokens[7].is<tscc::lex::tokens::double_minus_token>());
		CHECK(tokens[8].is<tscc::lex::tokens::ampersand_token>());
		CHECK(tokens[9].is<tscc::lex::tokens::caret_token>());
		CHECK(tokens[10].is<tscc::lex::tokens::tilde_token>());
	}

	SECTION("Comparison Operators") {
		auto tokens = tokenize("== === != !== < > <= >=");
		REQUIRE(tokens.size() == 8);
		CHECK(tokens[0].is<tscc::lex::tokens::double_eq_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::triple_eq_token>());
		CHECK(tokens[2].is<tscc::lex::tokens::exclamation_eq_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::exclamation_eq_eq_token>());
		CHECK(tokens[4].is<tscc::lex::tokens::less_token>());
		CHECK(tokens[5].is<tscc::lex::tokens::greater_token>());
		CHECK(tokens[6].is<tscc::lex::tokens::less_eq_token>());
		CHECK(tokens[7].is<tscc::lex::tokens::greater_eq_token>());
	}

	SECTION("Syntactic Operators") {
		auto tokens = tokenize("{...obj}");
		REQUIRE(tokens.size() == 4);
		CHECK(tokens[0].is<tscc::lex::tokens::open_brace_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::triple_dot_token>());
		CHECK(tokens[2].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[2]->to_string() == "obj");
		CHECK(tokens[3].is<tscc::lex::tokens::close_brace_token>());
	}

	SECTION("Bitwise Operators") {
		auto tokens = tokenize("& | << >> >>>");
		REQUIRE(tokens.size() == 5);
		CHECK(tokens[0].is<tscc::lex::tokens::ampersand_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::bar_token>());
		CHECK(tokens[2].is<tscc::lex::tokens::double_less_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::double_greater_token>());
		CHECK(tokens[4].is<tscc::lex::tokens::triple_greater_token>());
	}

	SECTION("Logical Operators") {
		auto tokens = tokenize("&& || ! ?? ?.");
		REQUIRE(tokens.size() == 5);
		CHECK(tokens[0].is<tscc::lex::tokens::double_ampersand_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::double_bar_token>());
		CHECK(tokens[2].is<tscc::lex::tokens::exclamation_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::double_question_token>());
		CHECK(tokens[4].is<tscc::lex::tokens::question_dot_token>());
	}

	SECTION("Compound Operators") {
		auto tokens = tokenize(
			"+= -= *= /= %= **= &= |= ^= <<= >>= &&= ||= ??= >>>=");
		REQUIRE(tokens.size() == 15);
		CHECK(tokens[0].is<tscc::lex::tokens::plus_eq_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::minus_eq_token>());
		CHECK(tokens[2].is<tscc::lex::tokens::asterisk_eq_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::slash_eq_token>());
		CHECK(tokens[4].is<tscc::lex::tokens::percent_eq_token>());
		CHECK(tokens[5].is<tscc::lex::tokens::double_asterisk_eq_token>());
		CHECK(tokens[6].is<tscc::lex::tokens::ampersand_eq_token>());
		CHECK(tokens[7].is<tscc::lex::tokens::bar_eq_token>());
		CHECK(tokens[8].is<tscc::lex::tokens::caret_eq_token>());
		CHECK(tokens[9].is<tscc::lex::tokens::double_less_eq_token>());
		CHECK(tokens[10].is<tscc::lex::tokens::double_greater_eq_token>());
		CHECK(
			tokens[11].is<tscc::lex::tokens::double_ampersand_eq_token>());
		CHECK(tokens[12].is<tscc::lex::tokens::double_bar_eq_token>());
		CHECK(tokens[13].is<tscc::lex::tokens::double_question_eq_token>());
		CHECK(tokens[14].is<tscc::lex::tokens::triple_greater_eq_token>());
	}

	SECTION("Brackets") {
		auto tokens = tokenize("[ ]");
		REQUIRE(tokens.size() == 2);
		CHECK(tokens[0].is<tscc::lex::tokens::open_bracket_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::close_bracket_token>());
	}
}