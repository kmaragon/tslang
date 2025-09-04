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

TEST_CASE("Keywords", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();

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