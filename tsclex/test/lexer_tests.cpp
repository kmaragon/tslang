/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2022. Keef Aragon
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
#include <tsclex/lexer.hpp>
#include <tsclex/token.hpp>
#include "fake_source.hpp"
#include <sstream>

#ifndef __FILE_NAME__
#  define __FILE_NAME__ __FILE__
#endif

TEST_CASE("Lexer", "[lexer]") {
	SECTION("Shebang") {
		SECTION("Single line shebang") {
			auto source = std::make_shared<fake_source>(__FILE_NAME__);
			std::stringstream file{"#!  /bin/bash"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 1);
			REQUIRE(tokens[0]->to_string() == "#!/bin/bash");
		}

		SECTION("Single line shebang with spaces and newline") {
			auto source = std::make_shared<fake_source>(__FILE_NAME__);
			std::stringstream file{"#!  /bin/bash   \n"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 1);
			REQUIRE(tokens[0]->to_string() == "#!/bin/bash");
		}
	}

	SECTION("Single line comment") {
		SECTION("Single line comment at EOF") {
			auto source = std::make_shared<fake_source>(__FILE_NAME__);
			std::stringstream file{"  // this is a comment"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 1);
			REQUIRE(tokens[0]->to_string() == "//this is a comment");
		}

		SECTION("Single line comment with spaces and newline") {
			auto source = std::make_shared<fake_source>(__FILE_NAME__);
			std::stringstream file{"  //~ this is a comment \t \n  "};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 1);
			REQUIRE(tokens[0]->to_string() == "//~ this is a comment");
		}
	}

	SECTION("Known Keywords") {
		SECTION("Class Declaration") {
			auto source = std::make_shared<fake_source>(__FILE_NAME__);
			std::stringstream file{R"(
class MyClass extends MyBase implements IMyInterface
{
    constructor()
    {
    }

	interfaceMethod(a: string, b: number): void
    {
    }
}
)"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 37);
			REQUIRE(tokens[0].is<tscc::lex::tokens::newline_token>());
			REQUIRE(tokens[1].is<tscc::lex::tokens::class_token>());
			REQUIRE(tokens[2].is<tscc::lex::tokens::identifier_token>());
			REQUIRE(tokens[2]->to_string() == "MyClass");
			REQUIRE(tokens[3].is<tscc::lex::tokens::extends_token>());
			REQUIRE(tokens[4].is<tscc::lex::tokens::identifier_token>());
			REQUIRE(tokens[4]->to_string() == "MyBase");
			REQUIRE(tokens[5].is<tscc::lex::tokens::implements_token>());
			REQUIRE(tokens[6].is<tscc::lex::tokens::identifier_token>());
			REQUIRE(tokens[6]->to_string() == "IMyInterface");
			REQUIRE(tokens[7].is<tscc::lex::tokens::newline_token>());
			REQUIRE(tokens[8].is<tscc::lex::tokens::open_brace_token>());
			REQUIRE(tokens[9].is<tscc::lex::tokens::newline_token>());
			REQUIRE(tokens[10].is<tscc::lex::tokens::constructor_token>());
			REQUIRE(tokens[11].is<tscc::lex::tokens::open_paren_token>());
			REQUIRE(tokens[12].is<tscc::lex::tokens::close_paren_token>());
			REQUIRE(tokens[13].is<tscc::lex::tokens::newline_token>());
			REQUIRE(tokens[14].is<tscc::lex::tokens::open_brace_token>());
			REQUIRE(tokens[15].is<tscc::lex::tokens::newline_token>());
			REQUIRE(tokens[16].is<tscc::lex::tokens::close_brace_token>());
			REQUIRE(tokens[17].is<tscc::lex::tokens::newline_token>());
			REQUIRE(tokens[18].is<tscc::lex::tokens::identifier_token>());
			REQUIRE(tokens[18]->to_string() == "interfaceMethod");
			REQUIRE(tokens[19].is<tscc::lex::tokens::open_paren_token>());
			REQUIRE(tokens[20].is<tscc::lex::tokens::identifier_token>());
			REQUIRE(tokens[20]->to_string() == "a");
			REQUIRE(tokens[21].is<tscc::lex::tokens::colon_token>());
			REQUIRE(tokens[22].is<tscc::lex::tokens::string_token>());
			REQUIRE(tokens[23].is<tscc::lex::tokens::comma_token>());
			REQUIRE(tokens[24].is<tscc::lex::tokens::identifier_token>());
			REQUIRE(tokens[24]->to_string() == "b");
			REQUIRE(tokens[25].is<tscc::lex::tokens::colon_token>());
			REQUIRE(tokens[26].is<tscc::lex::tokens::number_token>());
			REQUIRE(tokens[27].is<tscc::lex::tokens::close_paren_token>());
			REQUIRE(tokens[28].is<tscc::lex::tokens::colon_token>());
			REQUIRE(tokens[29].is<tscc::lex::tokens::void_token>());
			REQUIRE(tokens[30].is<tscc::lex::tokens::newline_token>());
			REQUIRE(tokens[31].is<tscc::lex::tokens::open_brace_token>());
			REQUIRE(tokens[32].is<tscc::lex::tokens::newline_token>());
			REQUIRE(tokens[33].is<tscc::lex::tokens::close_brace_token>());
			REQUIRE(tokens[34].is<tscc::lex::tokens::newline_token>());
			REQUIRE(tokens[35].is<tscc::lex::tokens::close_brace_token>());
			REQUIRE(tokens[36].is<tscc::lex::tokens::newline_token>());
		}
	}

	SECTION("Multi line comment") {
		SECTION("Unterminated Multi-line comment") {
			auto source = std::make_shared<fake_source>(__FILE_NAME__);
			std::stringstream file{"  /* this is a comment"};
			tscc::lex::lexer subject(file, source);

			auto should_throw = [&]() {
				return std::vector<tscc::lex::token>{subject.begin(),
													 subject.end()};
			};
			REQUIRE_THROWS(should_throw());
		}

		SECTION("Multi line non-jsdoc comment") {
			auto source = std::make_shared<fake_source>(__FILE_NAME__);
			std::stringstream file{R"(/*
this is a comment
And some more
*/
)"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 2);
			REQUIRE(tokens[0].is<tscc::lex::tokens::multiline_comment_token>());
			REQUIRE(tokens[0]->to_string() ==
					"/*\nthis is a comment\nAnd some more\n*/");
			REQUIRE(tokens[1].is<tscc::lex::tokens::newline_token>());
		}
	}

	SECTION("Native UTF-8 Handling") {
		SECTION("Two-byte sequence") {
			auto source = std::make_shared<fake_source>(__FILE_NAME__);
			std::stringstream file{R"( const varµ = 12; )"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 5);
			REQUIRE(tokens[0].is<tscc::lex::tokens::const_token>());
			REQUIRE(tokens[1].is<tscc::lex::tokens::identifier_token>());
			REQUIRE(tokens[1]->to_string() == "varµ");
			REQUIRE(tokens[2].is<tscc::lex::tokens::eq_token>());
			REQUIRE(tokens[3].is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(tokens[3]->to_string() == "12");
			REQUIRE(tokens[4].is<tscc::lex::tokens::semicolon_token>());
		}

		SECTION("Three-byte sequence") {
			auto source = std::make_shared<fake_source>(__FILE_NAME__);
			std::stringstream file{R"( const varァ = 314.195e-2; )"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 5);
			REQUIRE(tokens[0].is<tscc::lex::tokens::const_token>());
			REQUIRE(tokens[1].is<tscc::lex::tokens::identifier_token>());
			REQUIRE(tokens[1]->to_string() == "varァ");
			REQUIRE(tokens[2].is<tscc::lex::tokens::eq_token>());
			REQUIRE(tokens[3].is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(tokens[3]->to_string().substr(0, 7) == "3.14195");
			REQUIRE(tokens[4].is<tscc::lex::tokens::semicolon_token>());
		}

		SECTION("Four-byte sequence") {
			auto source = std::make_shared<fake_source>(__FILE_NAME__);
			std::stringstream file{R"( const unistr = "String with native 😀"; )"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 5);
			REQUIRE(tokens[0].is<tscc::lex::tokens::const_token>());
			REQUIRE(tokens[1].is<tscc::lex::tokens::identifier_token>());
			REQUIRE(tokens[1]->to_string() == "unistr");
			REQUIRE(tokens[2].is<tscc::lex::tokens::eq_token>());
			REQUIRE(tokens[3].is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(tokens[3]->to_string() == "\"String with native \\ud83d\\ude00\"");
			REQUIRE(tokens[4].is<tscc::lex::tokens::semicolon_token>());
		}
	}
}
