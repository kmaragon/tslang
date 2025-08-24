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

#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <tsclex/lexer.hpp>
#include <tsclex/token.hpp>
#include "fake_source.hpp"

#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

TEST_CASE("Lexer", "[lexer]") {
	std::stringstream file;
	auto source = std::make_shared<fake_source>(__FILE__);

	auto create_lexer = [&file, &source](const std::string& input) {
		file.clear();
		file << input;
		file.seekg(0, std::ios::beg);
		return tscc::lex::lexer(file, source);
	};

	auto tokenize = [&create_lexer](const std::string& input) {
		auto lexer = create_lexer(input);
		return std::vector<tscc::lex::token>{lexer.begin(), lexer.end()};
	};

	SECTION("Shebang") {
		SECTION("Single line shebang") {
			auto tokens = tokenize("#!  /bin/bash");
			REQUIRE(tokens.size() == 1);
			CHECK(tokens[0]->to_string() == "#!/bin/bash");
		}

		SECTION("Single line shebang with spaces and newline") {
			auto tokens = tokenize("#!  /bin/bash   \n");
			REQUIRE(tokens.size() == 1);
			CHECK(tokens[0]->to_string() == "#!/bin/bash");
		}

		SECTION("Shebang not at start of file") {
			auto lexer = create_lexer("const x = 1;\n#! /bin/bash");
			REQUIRE_THROWS(
				std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
		}
	}

	SECTION("Single line comment") {
		SECTION("Single line comment at EOF") {
			auto tokens = tokenize("  // this is a comment");
			REQUIRE(tokens.size() == 1);
			CHECK(tokens[0]->to_string() == "// this is a comment");
		}

		SECTION("Single line comment with spaces and newline") {
			auto tokens = tokenize("  //~ this is a comment \t \n  ");
			REQUIRE(tokens.size() == 1);
			CHECK(tokens[0]->to_string() == "// ~ this is a comment");
		}
	}

	SECTION("Known Keywords") {
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

	SECTION("Multi line comment") {
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

	SECTION("Native UTF-8 Handling") {
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

	SECTION("Operators") {
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

	SECTION("Additional Punctuation and Edge Cases") {
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

	SECTION("String Escape Edge Cases") {
		SECTION("Additional Escape Sequences") {
			auto tokens = tokenize("'\\0\\b\\f\\v\\r'");
			REQUIRE(tokens.size() == 1);
			REQUIRE(tokens[0].is<tscc::lex::tokens::constant_value_token>());

			auto& cv = static_cast<tscc::lex::tokens::constant_value_token&>(
				*tokens[0]);
			auto sv = cv.string_value();
			REQUIRE(sv);
			REQUIRE(sv->at(0) == 0);
			REQUIRE(sv->at(1) == '\b');
			REQUIRE(sv->at(2) == '\f');
			REQUIRE(sv->at(3) == '\v');
			REQUIRE(sv->at(4) == '\r');
		}

		SECTION("String Without Terminating Newline") {
			auto tokens = tokenize("'hello'");
			REQUIRE(tokens.size() == 1);
			CHECK(tokens[0].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[0]->to_string() == "'hello'");
		}
	}

	SECTION("Comment Edge Cases") {
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

	SECTION("Literals") {
		SECTION("String Literals differnt quotes") {
			auto tokens = tokenize(R"("string" 'string' "escaped\nstring")");
			REQUIRE(tokens.size() == 3);
			CHECK(tokens[0].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[0]->to_string() == "\"string\"");
			CHECK(tokens[1].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[1]->to_string() == "'string'");
			CHECK(tokens[2].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[2]->to_string() == "\"escaped\\nstring\"");
		}

		SECTION("Unterminated string literal") {
			// Edge cases and error cases
			auto lexer = create_lexer("const x = 'unterminated");
			REQUIRE_THROWS(
				std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
		}

		SECTION("Escaped newline in string literal") {
			auto tokens = tokenize("const x = 'line1\\\nline2';");
			REQUIRE(tokens.size() == 5);
			CHECK(tokens[0].is<tscc::lex::tokens::const_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[1]->to_string() == "x");
			CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[3]->to_string() == "'line1line2'");
			CHECK(tokens[4].is<tscc::lex::tokens::semicolon_token>());
		}

		SECTION("Escaped windows newline in string literal") {
			auto tokens = tokenize("const x = 'line1\\\r\nline2';");
			REQUIRE(tokens.size() == 5);
			CHECK(tokens[0].is<tscc::lex::tokens::const_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[1]->to_string() == "x");
			CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[3]->to_string() == "'line1line2'");
			CHECK(tokens[4].is<tscc::lex::tokens::semicolon_token>());
		}

		SECTION("Unicode escape string literal with full sequence") {
			auto tokens = tokenize("const x = '\\u{1F600}';");
			REQUIRE(tokens.size() == 5);
			CHECK(tokens[0].is<tscc::lex::tokens::const_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[1]->to_string() == "x");
			CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[3]->to_string() == "'\\ud83d\\ude00'");
			CHECK(tokens[4].is<tscc::lex::tokens::semicolon_token>());
		}

		SECTION("Unicode escape string literal with utf-16 sequence") {
			auto tokens = tokenize("const x = '\\ud83d\\ude00';");
			REQUIRE(tokens.size() == 5);
			CHECK(tokens[0].is<tscc::lex::tokens::const_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[1]->to_string() == "x");
			CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());
			REQUIRE(tokens[3].is<tscc::lex::tokens::constant_value_token>());
			auto constant_value =
				static_cast<tscc::lex::tokens::constant_value_token&>(
					*tokens[3]);
			CHECK(constant_value.string_value() == U"😀");
			CHECK(tokens[4].is<tscc::lex::tokens::semicolon_token>());
		}

		SECTION("Bad hex string literal") {
			auto lexer = create_lexer("const x = '\\x';");
			REQUIRE_THROWS(
				std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
		}

		SECTION("Bad unicode escape string literal") {
			auto lexer = create_lexer("const x = '\\u{110000}';");
			REQUIRE_THROWS(
				std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
		}

		SECTION("Number Literals") {
			SECTION("Decimal Numbers") {
				SECTION("Basic Integers") {
					auto tokens = tokenize("123 456 789");
					REQUIRE(tokens.size() == 3);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[0]->to_string() == "123");
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "456");
					CHECK(tokens[2]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[2]->to_string() == "789");
				}

				SECTION("Negative Integers") {
					auto tokens = tokenize("-123 -456 -789");
					REQUIRE(tokens.size() == 6);
					CHECK(tokens[0].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "123");
					CHECK(tokens[2].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[3]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[3]->to_string() == "456");
					CHECK(tokens[4].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[5]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[5]->to_string() == "789");
				}

				SECTION("Decimal Points") {
					auto tokens = tokenize("123.456 0.789 1.0");
					REQUIRE(tokens.size() == 3);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(strtod(tokens[0]->to_string().c_str(), nullptr) ==
						  123.456);
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(strtod(tokens[1]->to_string().c_str(), nullptr) ==
						  0.789);
					CHECK(tokens[2]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK((tokens[2]->to_string() == "1" ||
						   tokens[2]->to_string().starts_with("1.0")));
				}

				SECTION("Negative Decimal Points") {
					auto tokens = tokenize("-123.456 -0.789 -1.0");
					REQUIRE(tokens.size() == 6);
					CHECK(tokens[0].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(strtod(tokens[1]->to_string().c_str(), nullptr) ==
						  123.456);
					CHECK(tokens[2].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[3]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(strtod(tokens[3]->to_string().c_str(), nullptr) ==
						  0.789);
					CHECK(tokens[4].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[5]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK((tokens[5]->to_string() == "1" ||
						   tokens[5]->to_string().starts_with("1.0")));
				}

				SECTION("Scientific Notation") {
					auto tokens = tokenize("1e10 1.2e-1 0.5e+2");
					REQUIRE(tokens.size() == 3);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[0]->to_string() == "1e10");
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "1.2e-1");
					CHECK(tokens[2]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[2]->to_string() == "0.5e2");
				}

				SECTION("Negative Scientific Notation") {
					auto tokens = tokenize("-1e10 -1.2e-1 -0.5e+2");
					REQUIRE(tokens.size() == 6);
					CHECK(tokens[0].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "1e10");
					CHECK(tokens[2].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[3]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[3]->to_string() == "1.2e-1");
					CHECK(tokens[4].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[5]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[5]->to_string() == "0.5e2");
				}

				SECTION("Edge Cases") {
					auto tokens = tokenize(".123 123. 0.");
					REQUIRE(tokens.size() == 3);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(strtod(tokens[0]->to_string().c_str(), nullptr) ==
						  0.123);
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "123");
					CHECK(tokens[2]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[2]->to_string() == "0");
				}

				SECTION("Negative Edge Cases") {
					auto tokens = tokenize("-.123 -123. -0.");
					REQUIRE(tokens.size() == 6);
					CHECK(tokens[0].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(strtod(tokens[1]->to_string().c_str(), nullptr) ==
						  0.123);
					CHECK(tokens[2].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[3]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[3]->to_string() == "123");
					CHECK(tokens[4].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[5]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[5]->to_string() == "0");
				}

				SECTION("Decimal with Separators") {
					auto tokens = tokenize("1_234 1_234.567 1.2e4");
					REQUIRE(tokens.size() == 3);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[0]->to_string() == "1234");
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(strtod(tokens[1]->to_string().c_str(), nullptr) ==
						  1234.567);
					CHECK(tokens[2]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[2]->to_string() == "1.2e4");
				}

				SECTION("Negative Decimal with Separators") {
					auto tokens = tokenize("-1_234 -1_234.567 -1.2e4");
					REQUIRE(tokens.size() == 6);
					CHECK(tokens[0].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "1234");
					CHECK(tokens[2].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[3]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(strtod(tokens[3]->to_string().c_str(), nullptr) ==
						  1234.567);
					CHECK(tokens[4].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[5]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[5]->to_string() == "1.2e4");
				}

				SECTION("Invalid Decimal with Multiple Decimal Points") {
					auto lexer = create_lexer("123.456.789");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Decimal with Incomplete Scientific Notation") {
					auto lexer = create_lexer("1e");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION(
					"Invalid Decimal with Separator Starting Scientific "
					"Notation") {
					auto lexer = create_lexer("1e_2");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION(
					"Invalid Decimal with Incomplete Scientific Notation "
					"Exponent") {
					auto lexer = create_lexer("1e+");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Decimal with Double Separator") {
					auto lexer = create_lexer("1__2");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Decimal with Separator at End") {
					auto lexer = create_lexer("1_2_");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Decimal with Separator at End of Number") {
					auto lexer = create_lexer("123_");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Decimal with Separator After Decimal Point") {
					auto lexer = create_lexer("1.2_3");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Decimal with Separator in Exponent") {
					auto lexer = create_lexer("1.2e1_0");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION(
					"Invalid Decimal with Separator Adjacent to Decimal "
					"Point") {
					auto lexer = create_lexer("1_.3");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}
			}

			SECTION("Binary Numbers") {
				SECTION("Valid Binary") {
					auto tokens =
						tokenize("0b1010 0B1111 0b0000 0b0001_0000_0000");
					REQUIRE(tokens.size() == 4);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[0]->to_string() == "0b1010");
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "0b1111");
					CHECK(tokens[2]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[2]->to_string() == "0b0");
					CHECK(tokens[3]->to_string() == "0b100000000");
				}

				SECTION("Negative Binary") {
					auto tokens = tokenize("-0b1010 -0B1111 -0b0000");
					REQUIRE(tokens.size() == 6);
					CHECK(tokens[0].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "0b1010");
					CHECK(tokens[2].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[3]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[3]->to_string() == "0b1111");
					CHECK(tokens[4].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[5]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[5]->to_string() == "0b0");
				}

				SECTION("Invalid Binary with Non-Binary Digit") {
					auto lexer = create_lexer("0b2");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Binary with Missing Digits") {
					auto lexer = create_lexer("0b");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Binary with Separator at Start") {
					auto lexer = create_lexer("0b_101");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Binary with Separator at End") {
					auto lexer = create_lexer("0b101_");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}
			}

			SECTION("Octal Numbers") {
				SECTION("Valid Octal") {
					auto tokens = tokenize("0o777 0O123 0o0 0o1_23");
					REQUIRE(tokens.size() == 4);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[0]->to_string() == "0o777");
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "0o123");
					CHECK(tokens[2]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[2]->to_string() == "0o0");
					CHECK(tokens[3]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[3]->to_string() == "0o123");
				}

				SECTION("Negative Octal") {
					auto tokens = tokenize("-0o777 -0O123 -0o0");
					REQUIRE(tokens.size() == 6);
					CHECK(tokens[0].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "0o777");
					CHECK(tokens[2].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[3]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[3]->to_string() == "0o123");
					CHECK(tokens[4].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[5]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[5]->to_string() == "0o0");
				}

				SECTION("Legacy Octal Format") {
					auto tokens = tokenize("054 091");
					REQUIRE(tokens.size() == 2);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[0]->to_string() ==
						  "0o54");	// Legacy octal 054 is interpreted as 0o54
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() ==
						  "91");  // 091 is interpreted as decimal 91
				}

				SECTION("Negative Legacy Octal Format") {
					auto tokens = tokenize("-054 -091");
					REQUIRE(tokens.size() == 4);
					CHECK(tokens[0].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() ==
						  "0o54");	// Legacy octal -054 is interpreted as -0o54
					CHECK(tokens[2].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[3]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[3]->to_string() ==
						  "91");  // -091 is interpreted as decimal -91
				}

				SECTION("Invalid Octal with Non-Octal Digit") {
					auto lexer = create_lexer("0o8");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Octal with Missing Digits") {
					auto lexer = create_lexer("0o");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Octal with Separator at Start") {
					auto lexer = create_lexer("0o_123");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Octal with Separator at End") {
					auto lexer = create_lexer("0o123_");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}
			}

			SECTION("Hexadecimal Numbers") {
				SECTION("Valid Hex") {
					auto tokens = tokenize("0xFF 0x123 0X0");
					REQUIRE(tokens.size() == 3);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[0]->to_string() == "0xff");
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "0x123");
					CHECK(tokens[2]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[2]->to_string() == "0x0");
				}

				SECTION("Negative Hex") {
					auto tokens = tokenize("-0xFF -0x123 -0X0");
					REQUIRE(tokens.size() == 6);
					CHECK(tokens[0].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "0xff");
					CHECK(tokens[2].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[3]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[3]->to_string() == "0x123");
					CHECK(tokens[4].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[5]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[5]->to_string() == "0x0");
				}

				SECTION("Hex with Separators") {
					auto tokens = tokenize("0xF_F 0X1_2_3");
					REQUIRE(tokens.size() == 2);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[0]->to_string() == "0xff");
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "0x123");
				}

				SECTION("Negative Hex with Separators") {
					auto tokens = tokenize("-0xF_F -0X1_2_3");
					REQUIRE(tokens.size() == 4);
					CHECK(tokens[0].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "0xff");
					CHECK(tokens[2].is<tscc::lex::tokens::minus_token>());
					CHECK(tokens[3]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[3]->to_string() == "0x123");
				}

				SECTION("Invalid Hex with Non-Hex Digit") {
					auto lexer = create_lexer("0xG");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Hex with Double Separator") {
					auto lexer = create_lexer("0xF__F");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Hex with Missing Digits") {
					auto lexer = create_lexer("0x");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Hex with Separator at Start") {
					auto lexer = create_lexer("0x_123");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}

				SECTION("Invalid Hex with Separator at End") {
					auto lexer = create_lexer("0x123_");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}
			}

			SECTION("BigInt Numbers") {
				SECTION("Decimal BigInt") {
					auto tokens = tokenize("123n 0n");
					REQUIRE(tokens.size() == 2);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[0]->to_string() == "123n");
					auto constant_value0 =
						static_cast<tscc::lex::tokens::constant_value_token&>(
							*tokens[0]);
					CHECK(constant_value0.is_bigint() == true);
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "0n");
					auto constant_value1 =
						static_cast<tscc::lex::tokens::constant_value_token&>(
							*tokens[1]);
					CHECK(constant_value1.is_bigint() == true);
				}

				SECTION("Binary BigInt") {
					auto tokens = tokenize("0b1010n 0B1111n");
					REQUIRE(tokens.size() == 2);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[0]->to_string() == "0b1010n");
					auto constant_value0 =
						static_cast<tscc::lex::tokens::constant_value_token&>(
							*tokens[0]);
					CHECK(constant_value0.is_bigint() == true);
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "0b1111n");
					auto constant_value1 =
						static_cast<tscc::lex::tokens::constant_value_token&>(
							*tokens[1]);
					CHECK(constant_value1.is_bigint() == true);
				}

				SECTION("Octal BigInt") {
					auto tokens = tokenize("0o777n 0O123n");
					REQUIRE(tokens.size() == 2);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[0]->to_string() == "0o777n");
					auto constant_value0 =
						static_cast<tscc::lex::tokens::constant_value_token&>(
							*tokens[0]);
					CHECK(constant_value0.is_bigint() == true);
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "0o123n");
					auto constant_value1 =
						static_cast<tscc::lex::tokens::constant_value_token&>(
							*tokens[1]);
					CHECK(constant_value1.is_bigint() == true);
				}

				SECTION("Hexadecimal BigInt") {
					auto tokens = tokenize("0xFFn 0x123n");
					REQUIRE(tokens.size() == 2);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[0]->to_string() == "0xffn");
					auto constant_value0 =
						static_cast<tscc::lex::tokens::constant_value_token&>(
							*tokens[0]);
					CHECK(constant_value0.is_bigint() == true);
					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "0x123n");
					auto constant_value1 =
						static_cast<tscc::lex::tokens::constant_value_token&>(
							*tokens[1]);
					CHECK(constant_value1.is_bigint() == true);
				}

				SECTION("Mixed regular and BigInt numbers") {
					auto tokens = tokenize("123 123n 0xFF 0xFFn");
					REQUIRE(tokens.size() == 4);
					CHECK(tokens[0]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[0]->to_string() == "123");
					auto constant_value0 =
						static_cast<tscc::lex::tokens::constant_value_token&>(
							*tokens[0]);
					CHECK(constant_value0.is_bigint() == false);

					CHECK(tokens[1]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[1]->to_string() == "123n");
					auto constant_value1 =
						static_cast<tscc::lex::tokens::constant_value_token&>(
							*tokens[1]);
					CHECK(constant_value1.is_bigint() == true);

					CHECK(tokens[2]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[2]->to_string() == "0xff");
					auto constant_value2 =
						static_cast<tscc::lex::tokens::constant_value_token&>(
							*tokens[2]);
					CHECK(constant_value2.is_bigint() == false);

					CHECK(tokens[3]
							  .is<tscc::lex::tokens::constant_value_token>());
					CHECK(tokens[3]->to_string() == "0xffn");
					auto constant_value3 =
						static_cast<tscc::lex::tokens::constant_value_token&>(
							*tokens[3]);
					CHECK(constant_value3.is_bigint() == true);
				}

				SECTION("Invalid BigInt with Separator at End") {
					auto lexer = create_lexer("123_n");
					REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(),
																 lexer.end()});
				}
			}
		}

		SECTION("Boolean and Null") {
			auto tokens = tokenize("true false null undefined");
			REQUIRE(tokens.size() == 4);
			CHECK(tokens[0].is<tscc::lex::tokens::true_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::false_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::null_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::undefined_token>());
		}
	}

	SECTION("TypeScript Extensions") {
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

	SECTION("Lambdas") {
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

	SECTION("Template Literals") {
		SECTION("Basic Template Literal") {
			auto tokens = tokenize("`template literal`");
			REQUIRE(tokens.size() == 3);
			CHECK(
				tokens[0]
					.is<tscc::lex::tokens::interpolated_string_start_token>());
			CHECK(
				tokens[1]
					.is<tscc::lex::tokens::interpolated_string_chunk_token>());
			CHECK(tokens[1]->to_string() == "template literal");
			CHECK(tokens[2]
					  .is<tscc::lex::tokens::interpolated_string_end_token>());
		}

		SECTION("Template Literal with Expression") {
			auto tokens = tokenize("`value: ${x}`");
			REQUIRE(tokens.size() == 6);
			CHECK(
				tokens[0]
					.is<tscc::lex::tokens::interpolated_string_start_token>());
			CHECK(
				tokens[1]
					.is<tscc::lex::tokens::interpolated_string_chunk_token>());
			CHECK(tokens[1]->to_string() == "value: ");
			CHECK(tokens[2].is<tscc::lex::tokens::template_start_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[3]->to_string() == "x");
			CHECK(tokens[4].is<tscc::lex::tokens::template_end_token>());
			CHECK(tokens[5]
					  .is<tscc::lex::tokens::interpolated_string_end_token>());
		}

		SECTION("Nested Template Literals") {
			auto tokens = tokenize("`outer ${exec(`inner ${x}`)}`");
			REQUIRE(tokens.size() == 14);
			CHECK(
				tokens[0]
					.is<tscc::lex::tokens::interpolated_string_start_token>());
			CHECK(
				tokens[1]
					.is<tscc::lex::tokens::interpolated_string_chunk_token>());
			CHECK(tokens[1]->to_string() == "outer ");
			CHECK(tokens[2].is<tscc::lex::tokens::template_start_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[3]->to_string() == "exec");
			CHECK(tokens[4].is<tscc::lex::tokens::open_paren_token>());
			CHECK(
				tokens[5]
					.is<tscc::lex::tokens::interpolated_string_start_token>());
			CHECK(
				tokens[6]
					.is<tscc::lex::tokens::interpolated_string_chunk_token>());
			CHECK(tokens[6]->to_string() == "inner ");
			CHECK(tokens[7].is<tscc::lex::tokens::template_start_token>());
			CHECK(tokens[8].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[8]->to_string() == "x");
			CHECK(tokens[9].is<tscc::lex::tokens::template_end_token>());
			CHECK(tokens[10]
					  .is<tscc::lex::tokens::interpolated_string_end_token>());
			CHECK(tokens[11].is<tscc::lex::tokens::close_paren_token>());
			CHECK(tokens[12].is<tscc::lex::tokens::template_end_token>());
			CHECK(tokens[13]
					  .is<tscc::lex::tokens::interpolated_string_end_token>());
		}

		SECTION("Template Literal with Expression And newline") {
			auto tokens = tokenize("`value: ${x}\n  also: ${y}`");
			REQUIRE(tokens.size() == 10);
			CHECK(
				tokens[0]
					.is<tscc::lex::tokens::interpolated_string_start_token>());
			CHECK(
				tokens[1]
					.is<tscc::lex::tokens::interpolated_string_chunk_token>());
			CHECK(tokens[1]->to_string() == "value: ");
			CHECK(tokens[2].is<tscc::lex::tokens::template_start_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[3]->to_string() == "x");
			CHECK(tokens[4].is<tscc::lex::tokens::template_end_token>());
			CHECK(
				tokens[5]
					.is<tscc::lex::tokens::interpolated_string_chunk_token>());
			CHECK(tokens[5]->to_string() == "\\n  also: ");
			CHECK(tokens[6].is<tscc::lex::tokens::template_start_token>());
			CHECK(tokens[7].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[7]->to_string() == "y");
			CHECK(tokens[8].is<tscc::lex::tokens::template_end_token>());
			CHECK(tokens[9]
					  .is<tscc::lex::tokens::interpolated_string_end_token>());
		}

		SECTION("Template Literal with Expression And CRLF") {
			auto tokens = tokenize("`value: ${x}\r\n  also: ${y}`");
			REQUIRE(tokens.size() == 10);
			CHECK(
				tokens[0]
					.is<tscc::lex::tokens::interpolated_string_start_token>());
			CHECK(
				tokens[1]
					.is<tscc::lex::tokens::interpolated_string_chunk_token>());
			CHECK(tokens[1]->to_string() == "value: ");
			CHECK(tokens[2].is<tscc::lex::tokens::template_start_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[3]->to_string() == "x");
			CHECK(tokens[4].is<tscc::lex::tokens::template_end_token>());
			CHECK(
				tokens[5]
					.is<tscc::lex::tokens::interpolated_string_chunk_token>());
			CHECK(tokens[5]->to_string() == "\\n  also: ");
			CHECK(tokens[6].is<tscc::lex::tokens::template_start_token>());
			CHECK(tokens[7].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[7]->to_string() == "y");
			CHECK(tokens[8].is<tscc::lex::tokens::template_end_token>());
			CHECK(tokens[9]
					  .is<tscc::lex::tokens::interpolated_string_end_token>());
		}

		SECTION("Unterminated string") {
			// Edge cases and error cases
			auto lexer = create_lexer("const x = `${");
			REQUIRE_THROWS(
				std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
		}
	}

	SECTION("Decorators") {
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

	SECTION("Keywords") {
		SECTION("Access Modifiers") {
			auto tokens = tokenize("public private protected");
			REQUIRE(tokens.size() == 3);
			CHECK(tokens[0].is<tscc::lex::tokens::public_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::private_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::protected_token>());
		}

		SECTION("Type Keywords") {
			auto tokens =
				tokenize("string number boolean any void null undefined");
			REQUIRE(tokens.size() == 7);
			CHECK(tokens[0].is<tscc::lex::tokens::string_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::number_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::boolean_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::any_token>());
			CHECK(tokens[4].is<tscc::lex::tokens::void_token>());
			CHECK(tokens[5].is<tscc::lex::tokens::null_token>());
			CHECK(tokens[6].is<tscc::lex::tokens::undefined_token>());
		}

		SECTION("Class Keywords") {
			auto tokens =
				tokenize("class interface extends implements abstract");
			REQUIRE(tokens.size() == 5);
			CHECK(tokens[0].is<tscc::lex::tokens::class_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::interface_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::extends_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::implements_token>());
			CHECK(tokens[4].is<tscc::lex::tokens::abstract_token>());
		}

		SECTION("Function Keywords") {
			auto tokens = tokenize("function return async await");
			REQUIRE(tokens.size() == 4);
			CHECK(tokens[0].is<tscc::lex::tokens::function_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::return_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::async_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::await_token>());
		}

		SECTION("Variable Keywords") {
			auto tokens = tokenize("var let const");
			REQUIRE(tokens.size() == 3);
			CHECK(tokens[0].is<tscc::lex::tokens::var_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::let_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::const_token>());
		}

		SECTION("Control Flow Keywords") {
			auto tokens = tokenize("if else switch case break continue");
			REQUIRE(tokens.size() == 6);
			CHECK(tokens[0].is<tscc::lex::tokens::if_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::else_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::switch_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::case_token>());
			CHECK(tokens[4].is<tscc::lex::tokens::break_token>());
			CHECK(tokens[5].is<tscc::lex::tokens::continue_token>());
		}

		SECTION("Loop Keywords") {
			auto tokens = tokenize("for while do");
			REQUIRE(tokens.size() == 3);
			CHECK(tokens[0].is<tscc::lex::tokens::for_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::while_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::do_token>());
		}

		SECTION("Error Handling Keywords") {
			auto tokens = tokenize("try catch finally throw");
			REQUIRE(tokens.size() == 4);
			CHECK(tokens[0].is<tscc::lex::tokens::try_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::catch_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::finally_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::throw_token>());
		}

		SECTION("Type System Keywords") {
			auto tokens = tokenize("type interface extends implements");
			REQUIRE(tokens.size() == 4);
			CHECK(tokens[0].is<tscc::lex::tokens::type_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::interface_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::extends_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::implements_token>());
		}

		SECTION("Import/Export Keywords") {
			auto tokens = tokenize("import export from default");
			REQUIRE(tokens.size() == 4);
			CHECK(tokens[0].is<tscc::lex::tokens::import_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::export_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::from_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::default_token>());
		}

		SECTION("Operator Keywords") {
			auto tokens = tokenize("instanceof typeof in new delete");
			REQUIRE(tokens.size() == 5);
			CHECK(tokens[0].is<tscc::lex::tokens::instanceof_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::typeof_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::in_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::new_token>());
			CHECK(tokens[4].is<tscc::lex::tokens::delete_token>());
		}

		SECTION("Advanced TypeScript Keywords") {
			auto tokens =
				tokenize("keyof infer satisfies assert namespace enum");
			REQUIRE(tokens.size() == 6);
			CHECK(tokens[0].is<tscc::lex::tokens::keyof_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::infer_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::satisfies_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::assert_token>());
			CHECK(tokens[4].is<tscc::lex::tokens::namespace_token>());
			CHECK(tokens[5].is<tscc::lex::tokens::enum_token>());
		}
	}
}
