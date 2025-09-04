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
#include <tsclex/error/conflicting_regex_flags.hpp>
#include <tsclex/error/regex_flag_unavailable.hpp>

TEST_CASE("Regular Expressions", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

	SECTION("Basic Regex Patterns") {
		auto tokens = tokenize("/abc/ /[a-z]+/ /\\d{2,4}/");
		REQUIRE(tokens.size() == 3);
		CHECK(tokens[0].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[0]->to_string() == "/abc/");
		CHECK(tokens[1].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[1]->to_string() == "/[a-z]+/");
		CHECK(tokens[2].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[2]->to_string() == "/\\d{2,4}/");
	}

	SECTION("Regex with Flags") {
		auto tokens = tokenize(
			"/test/g /pattern/i /multi/m /dotall/s /unicode/u /sticky/y");
		REQUIRE(tokens.size() == 6);

		// Global flag
		CHECK(tokens[0].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[0]->to_string() == "/test/g");
		auto& regex0 =
			static_cast<tscc::lex::tokens::regex_token&>(*tokens[0]);
		CHECK(has_flag(regex0.get_flags(),
					   tscc::lex::tokens::regex_token::flags::global));

		// Ignore case flag
		CHECK(tokens[1].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[1]->to_string() == "/pattern/i");
		auto& regex1 =
			static_cast<tscc::lex::tokens::regex_token&>(*tokens[1]);
		CHECK(has_flag(regex1.get_flags(),
					   tscc::lex::tokens::regex_token::flags::ignore_case));

		// Multiline flag
		CHECK(tokens[2].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[2]->to_string() == "/multi/m");
		auto& regex2 =
			static_cast<tscc::lex::tokens::regex_token&>(*tokens[2]);
		CHECK(has_flag(regex2.get_flags(),
					   tscc::lex::tokens::regex_token::flags::multiline));

		// Dot all flag
		CHECK(tokens[3].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[3]->to_string() == "/dotall/s");
		auto& regex3 =
			static_cast<tscc::lex::tokens::regex_token&>(*tokens[3]);
		CHECK(has_flag(regex3.get_flags(),
					   tscc::lex::tokens::regex_token::flags::dot_all));

		// Unicode flag
		CHECK(tokens[4].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[4]->to_string() == "/unicode/u");
		auto& regex4 =
			static_cast<tscc::lex::tokens::regex_token&>(*tokens[4]);
		CHECK(has_flag(regex4.get_flags(),
					   tscc::lex::tokens::regex_token::flags::unicode));

		// Sticky flag
		CHECK(tokens[5].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[5]->to_string() == "/sticky/y");
		auto& regex5 =
			static_cast<tscc::lex::tokens::regex_token&>(*tokens[5]);
		CHECK(has_flag(regex5.get_flags(),
					   tscc::lex::tokens::regex_token::flags::sticky));
	}

	SECTION("Regex with Multiple Flags") {
		auto tokens = tokenize("/pattern/gi /test/msu");
		REQUIRE(tokens.size() == 2);

		// Global + ignore case
		CHECK(tokens[0].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[0]->to_string() == "/pattern/ig");
		auto& regex0 =
			static_cast<tscc::lex::tokens::regex_token&>(*tokens[0]);
		CHECK(has_flag(regex0.get_flags(),
					   tscc::lex::tokens::regex_token::flags::global));
		CHECK(has_flag(regex0.get_flags(),
					   tscc::lex::tokens::regex_token::flags::ignore_case));

		// Multiline + dot all + unicode
		CHECK(tokens[1].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[1]->to_string() == "/test/msu");
		auto& regex1 =
			static_cast<tscc::lex::tokens::regex_token&>(*tokens[1]);
		CHECK(has_flag(regex1.get_flags(),
					   tscc::lex::tokens::regex_token::flags::multiline));
		CHECK(has_flag(regex1.get_flags(),
					   tscc::lex::tokens::regex_token::flags::dot_all));
		CHECK(has_flag(regex1.get_flags(),
					   tscc::lex::tokens::regex_token::flags::unicode));
	}

	SECTION("Regex with Escaped Characters") {
		auto tokens =
			tokenize(R"(/\// /\\./ /\n\t\r/ /[\n-A]+/ /\x41\u0042/)");
		REQUIRE(tokens.size() == 5);

		// Escaped forward slash
		CHECK(tokens[0].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[0]->to_string() == "/\\//");

		// Escaped backslash and dot
		CHECK(tokens[1].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[1]->to_string() == "/\\\\./");

		// Common escape sequences
		CHECK(tokens[2].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[2]->to_string() == "/\\n\\t\\r/");

		// Escape sequences in character classes
		CHECK(tokens[3].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[3]->to_string() == "/[\\n-A]+/");

		// Hex and unicode escapes
		CHECK(tokens[4].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[4]->to_string() == "/AB/");
	}

	SECTION("Regex in Context") {
		auto tokens = tokenize("const pattern = /[a-zA-Z]+/g;");
		REQUIRE(tokens.size() == 5);
		CHECK(tokens[0].is<tscc::lex::tokens::const_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[1]->to_string() == "pattern");
		CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());
		CHECK(tokens[3].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens[3]->to_string() == "/[a-zA-Z]+/g");
		CHECK(tokens[4].is<tscc::lex::tokens::semicolon_token>());
	}

	SECTION("Regex vs Division Disambiguation") {
		// After assignment, should be regex
		auto tokens1 = tokenize("x = /pattern/");
		REQUIRE(tokens1.size() == 3);
		CHECK(tokens1[0].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens1[0]->to_string() == "x");
		CHECK(tokens1[1].is<tscc::lex::tokens::eq_token>());
		CHECK(tokens1[2].is<tscc::lex::tokens::regex_token>());

		// After return, should be regex
		auto tokens2 = tokenize("return /pattern/");
		REQUIRE(tokens2.size() == 2);
		CHECK(tokens2[0].is<tscc::lex::tokens::return_token>());
		CHECK(tokens2[1].is<tscc::lex::tokens::regex_token>());

		// After open paren, should be regex
		auto tokens3 = tokenize("(/pattern/)");
		REQUIRE(tokens3.size() == 3);
		CHECK(tokens3[0].is<tscc::lex::tokens::open_paren_token>());
		CHECK(tokens3[1].is<tscc::lex::tokens::regex_token>());
		CHECK(tokens3[2].is<tscc::lex::tokens::close_paren_token>());
	}

	SECTION("Complex Regex Patterns") {
		auto tokens = tokenize(
			"/^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]"
			"|2[0-4][0-9]|[01]?[0-9][0-9]?)$/");
		REQUIRE(tokens.size() == 1);
		CHECK(tokens[0].is<tscc::lex::tokens::regex_token>());
		// This is an IP address validation regex pattern
		CHECK(tokens[0]->to_string() ==
			  "/^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-"
			  "5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/");
	}

	SECTION("Regex Error Cases") {
		SECTION("Invalid Regex Flags") {
			auto lexer = create_lexer("/pattern/xyz");
			REQUIRE_THROWS(
				std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
		}

		SECTION("Duplicate Regex Flags") {
			auto lexer = create_lexer("/pattern/gg");
			REQUIRE_THROWS(
				std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
		}

		constexpr auto construct_vector = [](auto& lexer) {
			return std::vector<tscc::lex::token>{lexer.begin(),
												 lexer.end()};
		};

		SECTION("Version-Specific Flag Availability") {
			// Test 's' flag unavailable in ES5
			auto lexer_es5 = create_lexer("/pattern/s",
										  tscc::lex::language_version::es5);
			REQUIRE_THROWS_AS(construct_vector(lexer_es5),
							  tscc::lex::regex_flag_unavailable);

			// Test 'u' and 'y' flags unavailable in ES3
			auto lexer_es3_u = create_lexer(
				"/pattern/u", tscc::lex::language_version::es3);
			REQUIRE_THROWS_AS(construct_vector(lexer_es3_u),
							  tscc::lex::regex_flag_unavailable);

			auto lexer_es3_y = create_lexer(
				"/pattern/y", tscc::lex::language_version::es3);
			REQUIRE_THROWS_AS(construct_vector(lexer_es3_y),
							  tscc::lex::regex_flag_unavailable);

			// Test 'v' flag unavailable in ES2022
			auto lexer_es2022 = create_lexer(
				"/pattern/v", tscc::lex::language_version::es2022);
			REQUIRE_THROWS_AS(construct_vector(lexer_es2022),
							  tscc::lex::regex_flag_unavailable);
		}

		SECTION("Conflicting Unicode Flags") {
			// Test 'u' and 'v' flags cannot be used together
			auto lexer_uv = create_lexer(
				"/pattern/uv", tscc::lex::language_version::es_next);
			REQUIRE_THROWS_AS(construct_vector(lexer_uv),
							  tscc::lex::conflicting_regex_flags);

			auto lexer_vu = create_lexer(
				"/pattern/vu", tscc::lex::language_version::es_next);
			REQUIRE_THROWS_AS(construct_vector(lexer_vu),
							  tscc::lex::conflicting_regex_flags);
		}
	}
}