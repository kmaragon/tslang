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

TEST_CASE("Multi line comment", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

	SECTION("Unterminated Multi-line comment") {
		auto lexer = create_lexer("  /* this is a comment");
		REQUIRE_THROWS(
			std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
	}

	SECTION("Multi line non-jsdoc comment") {
		auto tokens = tokenize(R"(/*
this is a comment
And some more
*/
)");
		REQUIRE(tokens.size() == 2);
		CHECK(tokens[0].is<tscc::lex::tokens::multiline_comment_token>());
		CHECK(tokens[0]->to_string() ==
			  "/*\nthis is a comment\nAnd some more\n*/");
		CHECK(tokens[1].is<tscc::lex::tokens::newline_token>());
	}

	SECTION("JSDoc Comment") {
		auto tokens = tokenize(R"(/**
 * @param {string} name - The name parameter
 * @returns {number} The result
 */
)");
		REQUIRE(tokens.size() == 2);
		CHECK(tokens[0].is<tscc::lex::tokens::jsdoc_token>());
		CHECK(tokens[0]->to_string() ==
			  "/**\n * @param {string} name - The name parameter\n * "
			  "@returns {number} The result\n */");
		CHECK(tokens[1].is<tscc::lex::tokens::newline_token>());
	}

	SECTION("Inline JSDoc Comment") {
		auto tokens = tokenize(
			R"(/** @returns {number} The result */ function get_result())");
		REQUIRE(tokens.size() == 5);
		CHECK(tokens[0].is<tscc::lex::tokens::jsdoc_token>());
		CHECK(tokens[0]->to_string() ==
			  "/** @returns {number} The result */");
		CHECK(tokens[1].is<tscc::lex::tokens::function_token>());
		CHECK(tokens[2].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[2]->to_string() == "get_result");
		CHECK(tokens[3].is<tscc::lex::tokens::open_paren_token>());
		CHECK(tokens[4].is<tscc::lex::tokens::close_paren_token>());
	}

	SECTION("Conflict Markers") {
		// Need to put the conflict markers in one line otherwise the text
		// editors freak out
		auto tokens = tokenize(
			"\n<<<<<<< HEAD\n=======\n>>>>>>> branch\n||||||| "
			"base\n=======\n>>>>>>> branch\n");
		REQUIRE(tokens.size() == 7);
		CHECK(tokens[0].is<tscc::lex::tokens::newline_token>());
		CHECK(tokens[1]
				  .is<tscc::lex::tokens::conflict_marker_trivia_token>());
		CHECK(tokens[1]->to_string() == "<<<<<<< HEAD");
		CHECK(tokens[2]
				  .is<tscc::lex::tokens::conflict_marker_trivia_token>());
		CHECK(tokens[2]->to_string() == "=======");
		CHECK(tokens[3]
				  .is<tscc::lex::tokens::conflict_marker_trivia_token>());
		CHECK(tokens[3]->to_string() == ">>>>>>> branch");
		CHECK(tokens[4]
				  .is<tscc::lex::tokens::conflict_marker_trivia_token>());
		CHECK(tokens[4]->to_string() == "||||||| base");
		CHECK(tokens[5]
				  .is<tscc::lex::tokens::conflict_marker_trivia_token>());
		CHECK(tokens[5]->to_string() == "=======");
		CHECK(tokens[6]
				  .is<tscc::lex::tokens::conflict_marker_trivia_token>());
		CHECK(tokens[6]->to_string() == ">>>>>>> branch");
	}
}