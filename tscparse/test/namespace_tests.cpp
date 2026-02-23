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
#include <tsclex/tokens/module_token.hpp>
#include <tsclex/tokens/namespace_token.hpp>
#include <tscparse/ast/declare_module_node.hpp>
#include <tscparse/ast/import_node.hpp>
#include <tscparse/ast/namespace_node.hpp>
#include <tscparse/error/declare_in_ambient_context.hpp>
#include <tscparse/error/expected_token.hpp>
#include <tscparse/error/unexpected_end_of_text.hpp>
#include "test_helpers.hpp"

static const tscc::parse::ast::namespace_node& parse_namespace(
	const std::string& input) {
	return test_utils::parse_node<tscc::parse::ast::namespace_node>(input);
}

TEST_CASE("namespace", "[parser][namespace]") {
	SECTION("namespace empty body") {
		auto& node = parse_namespace("namespace Foo { }");

		REQUIRE(node.declare_keyword() == nullptr);
		REQUIRE(!node.ambient());
		REQUIRE(node.keyword() != nullptr);
		REQUIRE(node.keyword()->is<tscc::lex::tokens::namespace_token>());
		REQUIRE(node.name().size() == 1);
		REQUIRE(node.name()[0].value() == "Foo");
		REQUIRE(node.children().empty());
	}

	SECTION("namespace dotted name") {
		auto& node = parse_namespace("namespace A.B.C { }");

		REQUIRE(node.name().size() == 3);
		REQUIRE(node.name()[0].value() == "A");
		REQUIRE(node.name()[1].value() == "B");
		REQUIRE(node.name()[2].value() == "C");
		REQUIRE(node.children().empty());
	}

	SECTION("module keyword") {
		auto& node = parse_namespace("module Foo { }");

		REQUIRE(node.keyword()->is<tscc::lex::tokens::module_token>());
		REQUIRE(node.name().size() == 1);
		REQUIRE(node.name()[0].value() == "Foo");
		REQUIRE(!node.ambient());
	}

	SECTION("declare namespace") {
		auto& node = parse_namespace("declare namespace Foo { }");

		REQUIRE(node.declare_keyword() != nullptr);
		REQUIRE(node.declare_keyword()->is<tscc::lex::tokens::declare_token>());
		REQUIRE(node.keyword()->is<tscc::lex::tokens::namespace_token>());
		REQUIRE(node.ambient());
		REQUIRE(node.name()[0].value() == "Foo");
	}

	SECTION("declare module with identifier") {
		auto& node = parse_namespace("declare module Foo { }");

		REQUIRE(node.declare_keyword() != nullptr);
		REQUIRE(node.keyword()->is<tscc::lex::tokens::module_token>());
		REQUIRE(node.ambient());
		REQUIRE(node.name()[0].value() == "Foo");
	}

	SECTION("nested namespace child") {
		auto& node = parse_namespace("namespace Foo { namespace Bar { } }");

		REQUIRE(node.children().size() == 1);

		auto* inner = dynamic_cast<const tscc::parse::ast::namespace_node*>(
			node.children()[0].get());
		REQUIRE(inner != nullptr);
		REQUIRE(inner->parent() == &node);
		REQUIRE(inner->name()[0].value() == "Bar");
	}

	SECTION("namespace inside declare module") {
		auto& outer =
			test_utils::parse_node<tscc::parse::ast::declare_module_node>(
				"declare module \"outer\" { namespace Inner { } }");

		REQUIRE(outer.children().size() == 1);

		auto* inner = dynamic_cast<const tscc::parse::ast::namespace_node*>(
			outer.children()[0].get());
		REQUIRE(inner != nullptr);
		REQUIRE(inner->parent() == &outer);
		REQUIRE(inner->name()[0].value() == "Inner");
	}

	SECTION("import restrictions in namespace") {
		SECTION("side-effect import is illegal") {
			REQUIRE_THROWS_AS(
				parse_namespace("namespace Foo { import \"bar\"; }"),
				tscc::parse::expected_token);
		}

		SECTION("equals import is legal") {
			auto& node = parse_namespace("namespace Foo { import X = A.B; }");

			REQUIRE(node.children().size() == 1);

			auto* imp = dynamic_cast<const tscc::parse::ast::import_node*>(
				node.children()[0].get());
			REQUIRE(imp != nullptr);
			REQUIRE(imp->equals_name().value() == "X");
			REQUIRE(imp->entity_name().size() == 2);
			REQUIRE(imp->entity_name()[0].value() == "A");
			REQUIRE(imp->entity_name()[1].value() == "B");
		}

		SECTION("side-effect import illegal with module keyword") {
			REQUIRE_THROWS_AS(parse_namespace("module Foo { import \"bar\"; }"),
							  tscc::parse::expected_token);
		}
	}

	SECTION("parse errors") {
		SECTION("declare inside ambient namespace (TS1038)") {
			try {
				parse_namespace(
					"declare namespace Foo { declare namespace Bar { } }");
				FAIL("Expected declare_in_ambient_context");
			} catch (const tscc::parse::declare_in_ambient_context& e) {
				REQUIRE(e.code() == tscc::error_code::ts1038);
			}
		}

		SECTION("missing identifier") {
			REQUIRE_THROWS_AS(parse_namespace("namespace { }"),
							  tscc::parse::expected_token);
		}

		SECTION("missing open brace") {
			REQUIRE_THROWS_AS(parse_namespace("namespace Foo }"),
							  tscc::parse::expected_token);
		}

		SECTION("unclosed brace") {
			REQUIRE_THROWS_AS(parse_namespace("namespace Foo {"),
							  tscc::parse::unexpected_end_of_text);
		}
	}
}

// TODO: Tests blocked on declaration states that don't exist yet:
// - declare namespace Foo { function bar(): void; } — signature, should succeed
// - declare namespace Foo { function bar() { } } — body, should error
// - declare namespace Foo { class C { method(): void; } } — should succeed
// - declare namespace Foo { class C { method() { } } } — should error
