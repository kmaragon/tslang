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
#include <tsclex/lexer.hpp>
#include <tsclex/token.hpp>
#include "fake_source.hpp"
#include <sstream>

#ifndef __FILE_NAME__
#  define __FILE_NAME__ __FILE__
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
			auto source = std::make_shared<fake_source>(__FILE__);
			std::stringstream file{"#!  /bin/bash"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 1);
			CHECK(tokens[0]->to_string() == "#!/bin/bash");
		}

		SECTION("Single line shebang with spaces and newline") {
			auto source = std::make_shared<fake_source>(__FILE__);
			std::stringstream file{"#!  /bin/bash   \n"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 1);
			CHECK(tokens[0]->to_string() == "#!/bin/bash");
		}
	}

	SECTION("Single line comment") {
		SECTION("Single line comment at EOF") {
			auto source = std::make_shared<fake_source>(__FILE__);
			std::stringstream file{"  // this is a comment"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 1);
			CHECK(tokens[0]->to_string() == "//this is a comment");
		}

		SECTION("Single line comment with spaces and newline") {
			auto source = std::make_shared<fake_source>(__FILE__);
			std::stringstream file{"  //~ this is a comment \t \n  "};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 1);
			CHECK(tokens[0]->to_string() == "//~ this is a comment");
		}
	}

	SECTION("Known Keywords") {
		SECTION("Class Declaration") {
			auto source = std::make_shared<fake_source>(__FILE__);
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
			auto source = std::make_shared<fake_source>(__FILE__);
			std::stringstream file{"  /* this is a comment"};
			tscc::lex::lexer subject(file, source);

			auto should_throw = [&]() {
				return std::vector<tscc::lex::token>{subject.begin(),
													 subject.end()};
			};
			REQUIRE_THROWS(should_throw());
		}

		SECTION("Multi line non-jsdoc comment") {
			auto source = std::make_shared<fake_source>(__FILE__);
			std::stringstream file{R"(/*
this is a comment
And some more
*/
)"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 2);
			CHECK(tokens[0].is<tscc::lex::tokens::multiline_comment_token>());
			CHECK(tokens[0]->to_string() ==
					"/*\nthis is a comment\nAnd some more\n*/");
			CHECK(tokens[1].is<tscc::lex::tokens::newline_token>());
		}
	}

	SECTION("Native UTF-8 Handling") {
		SECTION("Two-byte sequence") {
			auto source = std::make_shared<fake_source>(__FILE__);
			std::stringstream file{R"( const varµ = 12; )"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
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
			auto source = std::make_shared<fake_source>(__FILE__);
			std::stringstream file{R"( const varァ = 314.195e-2; )"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 5);
			CHECK(tokens[0].is<tscc::lex::tokens::const_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[1]->to_string() == "varァ");
			CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[3]->to_string().substr(0, 7) == "3.14195");
			CHECK(tokens[4].is<tscc::lex::tokens::semicolon_token>());
		}

		SECTION("Four-byte sequence") {
			auto source = std::make_shared<fake_source>(__FILE__);
			std::stringstream file{R"( const unistr = "String with native 😀"; )"};
			tscc::lex::lexer subject(file, source);

			std::vector<tscc::lex::token> tokens{subject.begin(),
												 subject.end()};
			REQUIRE(tokens.size() == 5);
			CHECK(tokens[0].is<tscc::lex::tokens::const_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[1]->to_string() == "unistr");
			CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[3]->to_string() == "\"String with native \\ud83d\\ude00\"");
			CHECK(tokens[4].is<tscc::lex::tokens::semicolon_token>());
		}
	}

	SECTION("Operators") {
		SECTION("Arithmetic Operators") {
			auto tokens = tokenize("+ - * / % ** ++ --");
			REQUIRE(tokens.size() == 8);
			CHECK(tokens[0].is<tscc::lex::tokens::plus_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::minus_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::asterisk_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::slash_token>());
			CHECK(tokens[4].is<tscc::lex::tokens::percent_token>());
			CHECK(tokens[5].is<tscc::lex::tokens::double_asterisk_token>());
			CHECK(tokens[6].is<tscc::lex::tokens::double_plus_token>());
			CHECK(tokens[7].is<tscc::lex::tokens::double_minus_token>());
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
			auto tokens = tokenize("+= -= *= /= %= **=");
			REQUIRE(tokens.size() == 6);
			CHECK(tokens[0].is<tscc::lex::tokens::plus_eq_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::minus_eq_token>());
			CHECK(tokens[2].is<tscc::lex::tokens::asterisk_eq_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::slash_eq_token>());
			CHECK(tokens[4].is<tscc::lex::tokens::percent_eq_token>());
			CHECK(tokens[5].is<tscc::lex::tokens::double_asterisk_eq_token>());
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
			REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
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
			auto constant_value = static_cast<tscc::lex::tokens::constant_value_token&>(*tokens[3]);
			CHECK(constant_value.string_value() == U"😀");
			CHECK(tokens[4].is<tscc::lex::tokens::semicolon_token>());
		}

		SECTION("Bad hex string literal") {
			auto lexer = create_lexer("const x = '\\x';");
			REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
		}

		SECTION("Bad unicode escape string literal")
		{
			auto lexer = create_lexer("const x = '\\u{110000}';");
			REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
		}

		SECTION("Number Literals") {
			auto tokens = tokenize("123 123.456 1e10 0xFF 0b1010 0o777");
			REQUIRE(tokens.size() == 6);
			CHECK(tokens[0].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[0]->to_string() == "123");
			CHECK(tokens[1].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[1]->to_string() == "123.456");
			CHECK(tokens[2].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[2]->to_string() == "1e10");
			CHECK(tokens[3].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[3]->to_string() == "0xFF");
			CHECK(tokens[4].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[4]->to_string() == "0b1010");
			CHECK(tokens[5].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[5]->to_string() == "0o777");

			// Edge cases and error cases
			auto lexer = create_lexer("const x = 123.456.789");
			REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});

			auto lexer2 = create_lexer("const x = 0xG;");
			REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer2.begin(), lexer2.end()});

			auto lexer3 = create_lexer("const x = 0b2;");
			REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer3.begin(), lexer3.end()});

			auto lexer4 = create_lexer("const x = 0o8;");
			REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer4.begin(), lexer4.end()});
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

	SECTION("TypeScript Specific") {
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
			REQUIRE(tokens.size() == 8);
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

	SECTION("Template Literals") {
		SECTION("Basic Template Literal") {
			auto tokens = tokenize("`template literal`");
			REQUIRE(tokens.size() == 1);
			CHECK(tokens[0].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[0]->to_string() == "`template literal`");
		}

		SECTION("Template Literal with Expression") {
			auto tokens = tokenize("`value: ${x}`");
			REQUIRE(tokens.size() == 4);
			CHECK(tokens[0].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[0]->to_string() == "`value: `");
			CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[1]->to_string() == "x");
			CHECK(tokens[2].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[2]->to_string() == "``");
		}

		SECTION("Nested Template Literals") {
			auto tokens = tokenize("`outer ${`inner ${x}`}`");
			REQUIRE(tokens.size() == 6);
			CHECK(tokens[0].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[0]->to_string() == "`outer `");
			CHECK(tokens[1].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[1]->to_string() == "`inner `");
			CHECK(tokens[2].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[2]->to_string() == "x");
			CHECK(tokens[3].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[3]->to_string() == "``");
			CHECK(tokens[4].is<tscc::lex::tokens::constant_value_token>());
			CHECK(tokens[4]->to_string() == "``");
		}

		// Edge cases and error cases
		auto lexer = create_lexer("const x = `${");
		REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
	}

	SECTION("Decorators") {
		SECTION("Class Decorator") {
			auto tokens = tokenize("@decorator class MyClass {}");
			REQUIRE(tokens.size() == 5);
			CHECK(tokens[0].is<tscc::lex::tokens::at_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[1]->to_string() == "decorator");
			CHECK(tokens[2].is<tscc::lex::tokens::class_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[3]->to_string() == "MyClass");
			CHECK(tokens[4].is<tscc::lex::tokens::open_brace_token>());
		}

		SECTION("Method Decorator") {
			auto tokens = tokenize("@readonly get value() {}");
			REQUIRE(tokens.size() == 6);
			CHECK(tokens[0].is<tscc::lex::tokens::at_token>());
			CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[1]->to_string() == "readonly");
			CHECK(tokens[2].is<tscc::lex::tokens::get_token>());
			CHECK(tokens[3].is<tscc::lex::tokens::identifier_token>());
			CHECK(tokens[3]->to_string() == "value");
			CHECK(tokens[4].is<tscc::lex::tokens::open_paren_token>());
			CHECK(tokens[5].is<tscc::lex::tokens::close_paren_token>());
		}

		// Edge cases and error cases
		auto lexer = create_lexer("@decorator;");
		REQUIRE_THROWS(std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
	}
}
