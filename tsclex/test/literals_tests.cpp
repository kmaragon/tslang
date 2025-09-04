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

TEST_CASE("Literals", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

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