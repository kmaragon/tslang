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

TEST_CASE("Template Literals", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

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