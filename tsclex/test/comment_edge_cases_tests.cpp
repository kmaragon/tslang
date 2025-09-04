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

TEST_CASE("Comment Edge Cases", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

	SECTION("Comment at EOF without newline") {
		auto tokens = tokenize("let x = 1; // final comment");
		REQUIRE(tokens.size() == 6);
		CHECK(tokens[0].is<tscc::lex::tokens::let_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[1]->to_string() == "x");
		CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::constant_value_token>());
		CHECK(tokens[3]->to_string() == "1");
		CHECK(tokens[4].is<tscc::lex::tokens::semicolon_token>());
		CHECK(tokens[5].is<tscc::lex::tokens::comment_token>());
		CHECK(tokens[5]->to_string() == "// final comment");
	}

	SECTION("Unterminated JSDoc") {
		// Test for unterminated JSDoc comments
		auto lexer = create_lexer("/** @param {string} name");
		REQUIRE_THROWS(
			std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
	}
}