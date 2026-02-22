/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2026. Keef Aragon
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
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/declare_token.hpp>
#include <tsclex/tokens/module_token.hpp>
#include <tscparse/ast/declare_module_node.hpp>
#include <tscparse/ast/import_node.hpp>
#include <tscparse/error/expected_token.hpp>
#include <tscparse/error/unexpected_end_of_text.hpp>
#include "test_helpers.hpp"

static const tscc::parse::ast::declare_module_node& parse_declare_module(
	const std::string& input) {
	return test_utils::parse_node<tscc::parse::ast::declare_module_node>(input);
}

TEST_CASE("declare", "[parser][declare][module]") {
	SECTION("declare module empty body") {
		auto& node = parse_declare_module("declare module \"foo\" { }");

		REQUIRE(node.declare_keyword() != nullptr);
		REQUIRE(
			node.declare_keyword()->is<tscc::lex::tokens::declare_token>());
		REQUIRE(node.declare_keyword()->location().line() == 0);
		REQUIRE(node.declare_keyword()->location().column() == 0);
		REQUIRE(node.module_keyword() != nullptr);
		REQUIRE(
			node.module_keyword()->is<tscc::lex::tokens::module_token>());
		REQUIRE(node.module_name());
		REQUIRE(
			node.module_name()->is<tscc::lex::tokens::constant_value_token>());
		REQUIRE(node.module_name().value() == "foo");
		REQUIRE(node.children().empty());
	}

	SECTION("declare module with import") {
		auto& node = parse_declare_module(
			"declare module \"foo\" { import \"bar\"; }");

		REQUIRE(node.module_name().value() == "foo");
		REQUIRE(node.children().size() == 1);

		auto* import = dynamic_cast<const tscc::parse::ast::import_node*>(
			node.children()[0].get());
		REQUIRE(import != nullptr);
		REQUIRE(import->parent() == &node);
		REQUIRE(import->module_specifier());
		REQUIRE(import->module_specifier().value() == "bar");
	}

	SECTION("declare module with multiple children") {
		auto& node = parse_declare_module(
			"declare module \"foo\" { import \"a\"; import \"b\"; }");

		REQUIRE(node.module_name().value() == "foo");
		REQUIRE(node.children().size() == 2);

		auto* first = dynamic_cast<const tscc::parse::ast::import_node*>(
			node.children()[0].get());
		REQUIRE(first != nullptr);
		REQUIRE(first->parent() == &node);
		REQUIRE(first->module_specifier().value() == "a");

		auto* second = dynamic_cast<const tscc::parse::ast::import_node*>(
			node.children()[1].get());
		REQUIRE(second != nullptr);
		REQUIRE(second->parent() == &node);
		REQUIRE(second->module_specifier().value() == "b");
	}

	SECTION("parse errors") {
		SECTION("missing module name") {
			REQUIRE_THROWS_AS(parse_declare_module("declare module {"),
							  tscc::parse::expected_token);
		}

		SECTION("missing open brace") {
			REQUIRE_THROWS_AS(
				parse_declare_module("declare module \"foo\" import"),
				tscc::parse::expected_token);
		}

		SECTION("unclosed brace") {
			REQUIRE_THROWS_AS(
				parse_declare_module("declare module \"foo\" { import \"bar\";"),
				tscc::parse::unexpected_end_of_text);
		}
	}
}
