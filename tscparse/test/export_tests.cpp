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
#include <tsclex/tokens/declare_token.hpp>
#include <tsclex/tokens/export_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/module_token.hpp>
#include <tsclex/tokens/namespace_token.hpp>
#include <tsclex/tokens/type_token.hpp>
#include <tscparse/ast/import_node.hpp>
#include <tscparse/ast/namespace_node.hpp>
#include <tscparse/ast/type_node.hpp>
#include "test_helpers.hpp"

TEST_CASE("export", "[parser][export]") {
	SECTION("export type") {
		SECTION("export type Foo = string;") {
			auto& node =
				test_utils::parse_node<tscc::parse::ast::type_node>(
					"export type Foo = string;");

			REQUIRE(node.export_keyword() != nullptr);
			REQUIRE(
				node.export_keyword()->is<tscc::lex::tokens::export_token>());
			REQUIRE(node.export_keyword()->location().line() == 0);
			REQUIRE(node.export_keyword()->location().column() == 0);
			REQUIRE(node.declare_keyword() == nullptr);
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(node.keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.name());
			REQUIRE(node.name().value() == "Foo");
		}

		SECTION("ASI: export type at EOF") {
			auto& node =
				test_utils::parse_node<tscc::parse::ast::type_node>(
					"export type Foo = string");

			REQUIRE(node.export_keyword() != nullptr);
			REQUIRE(node.name().value() == "Foo");
		}

		SECTION("ASI: two exported types on separate lines") {
			auto r = test_utils::parse_source(
				"export type Foo = string\nexport type Bar = number");
			REQUIRE(r.root != nullptr);
			REQUIRE(r.root->children().size() == 2);

			auto* first = dynamic_cast<const tscc::parse::ast::type_node*>(
				r.root->children()[0].get());
			REQUIRE(first != nullptr);
			REQUIRE(first->export_keyword() != nullptr);
			REQUIRE(first->name().value() == "Foo");

			auto* second = dynamic_cast<const tscc::parse::ast::type_node*>(
				r.root->children()[1].get());
			REQUIRE(second != nullptr);
			REQUIRE(second->export_keyword() != nullptr);
			REQUIRE(second->name().value() == "Bar");
		}
	}

	SECTION("export namespace") {
		SECTION("export namespace Foo { }") {
			auto& node =
				test_utils::parse_node<tscc::parse::ast::namespace_node>(
					"export namespace Foo { }");

			REQUIRE(node.export_keyword() != nullptr);
			REQUIRE(
				node.export_keyword()->is<tscc::lex::tokens::export_token>());
			REQUIRE(node.declare_keyword() == nullptr);
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(
				node.keyword()->is<tscc::lex::tokens::namespace_token>());
			REQUIRE(node.name().size() == 1);
			REQUIRE(node.name()[0].value() == "Foo");
			REQUIRE(node.children().empty());
		}

		SECTION("export module Foo { }") {
			auto& node =
				test_utils::parse_node<tscc::parse::ast::namespace_node>(
					"export module Foo { }");

			REQUIRE(node.export_keyword() != nullptr);
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(node.keyword()->is<tscc::lex::tokens::module_token>());
			REQUIRE(node.name().size() == 1);
			REQUIRE(node.name()[0].value() == "Foo");
		}
	}

	SECTION("export import") {
		SECTION("export import x = A.B.C;") {
			auto& node =
				test_utils::parse_node<tscc::parse::ast::import_node>(
					"export import x = A.B.C;");

			REQUIRE(node.export_keyword() != nullptr);
			REQUIRE(
				node.export_keyword()->is<tscc::lex::tokens::export_token>());
			REQUIRE(node.equals_name());
			REQUIRE(node.equals_name().value() == "x");
			REQUIRE(node.entity_name().size() == 3);
			REQUIRE(node.entity_name()[0].value() == "A");
			REQUIRE(node.entity_name()[1].value() == "B");
			REQUIRE(node.entity_name()[2].value() == "C");
		}
	}

	SECTION("export declare") {
		SECTION("export declare type Foo = string;") {
			auto& node =
				test_utils::parse_node<tscc::parse::ast::type_node>(
					"export declare type Foo = string;");

			REQUIRE(node.export_keyword() != nullptr);
			REQUIRE(
				node.export_keyword()->is<tscc::lex::tokens::export_token>());
			REQUIRE(node.declare_keyword() != nullptr);
			REQUIRE(
				node.declare_keyword()->is<tscc::lex::tokens::declare_token>());
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(node.keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.name().value() == "Foo");
		}

		SECTION("export declare namespace Foo { }") {
			auto& node =
				test_utils::parse_node<tscc::parse::ast::namespace_node>(
					"export declare namespace Foo { }");

			REQUIRE(node.export_keyword() != nullptr);
			REQUIRE(
				node.export_keyword()->is<tscc::lex::tokens::export_token>());
			REQUIRE(node.declare_keyword() != nullptr);
			REQUIRE(
				node.declare_keyword()->is<tscc::lex::tokens::declare_token>());
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(
				node.keyword()->is<tscc::lex::tokens::namespace_token>());
			REQUIRE(node.name().size() == 1);
			REQUIRE(node.name()[0].value() == "Foo");
		}
	}

	SECTION("nested export") {
		SECTION("export namespace with exported type inside") {
			auto& node =
				test_utils::parse_node<tscc::parse::ast::namespace_node>(
					"export namespace Foo { export type Bar = string; }");

			REQUIRE(node.export_keyword() != nullptr);
			REQUIRE(node.name().size() == 1);
			REQUIRE(node.name()[0].value() == "Foo");
			REQUIRE(node.children().size() == 1);

			auto* inner = dynamic_cast<const tscc::parse::ast::type_node*>(
				node.children()[0].get());
			REQUIRE(inner != nullptr);
			REQUIRE(inner->export_keyword() != nullptr);
			REQUIRE(inner->name().value() == "Bar");
			REQUIRE(inner->parent() == &node);
		}
	}
}
