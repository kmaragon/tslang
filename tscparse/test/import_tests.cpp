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
#include <tsclex/tokens/assert_token.hpp>
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/default_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/import_token.hpp>
#include <tsclex/tokens/type_token.hpp>
#include <tsclex/tokens/with_token.hpp>
#include <tscparse/ast/import_node.hpp>
#include <tscparse/error/expected_token.hpp>
#include <tscparse/error/unexpected_end_of_text.hpp>
#include "test_helpers.hpp"

static const tscc::parse::ast::import_node& parse_import(
	const std::string& input) {
	return test_utils::parse_node<tscc::parse::ast::import_node>(input);
}

static std::optional<std::u32string_view> string_value(
	const tscc::lex::token* tok) {
	return static_cast<const tscc::lex::tokens::constant_value_token&>(**tok)
		.string_value();
}

static std::optional<std::u32string_view> string_value(
	const tscc::lex::token& tok) {
	return static_cast<const tscc::lex::tokens::constant_value_token&>(*tok)
		.string_value();
}

TEST_CASE("import", "[parser][import]") {
	SECTION("side-effect") {
		SECTION("import \"module\";") {
			auto& node = parse_import("import \"module\";");

			REQUIRE(
				node.import_keyword()->is<tscc::lex::tokens::import_token>());
			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.require_module_specifier() == nullptr);
			REQUIRE(node.entity_identifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
			REQUIRE(node.attributes().empty());
		}

		SECTION("import 'module';") {
			auto& node = parse_import("import 'module';");

			REQUIRE(
				node.import_keyword()->is<tscc::lex::tokens::import_token>());
			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.require_module_specifier() == nullptr);
			REQUIRE(node.entity_identifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
			REQUIRE(node.attributes().empty());
		}
	}

	SECTION("default binding") {
		SECTION("import foo from \"module\";") {
			auto& node = parse_import("import foo from \"module\";");

			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "foo");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.require_module_specifier() == nullptr);
			REQUIRE(node.entity_identifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
			REQUIRE(node.attributes().empty());
		}
	}

	SECTION("namespace binding") {
		SECTION("import * as ns from \"module\";") {
			auto& node = parse_import("import * as ns from \"module\";");

			REQUIRE(node.namespace_name() != nullptr);
			REQUIRE(node.namespace_name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.namespace_name())->to_string() == "ns");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.require_module_specifier() == nullptr);
			REQUIRE(node.entity_identifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
			REQUIRE(node.attributes().empty());
		}
	}

	SECTION("named bindings") {
		SECTION("import { foo } from \"module\";") {
			auto& node = parse_import("import { foo } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 1);

			auto& spec = node.named_specifiers()[0];
			REQUIRE_FALSE(spec.type_keyword.has_value());
			REQUIRE(spec.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name->to_string() == "foo");
			REQUIRE_FALSE(spec.alias.has_value());

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.require_module_specifier() == nullptr);
			REQUIRE(node.entity_identifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
			REQUIRE(node.attributes().empty());
		}

		SECTION("import { foo, bar } from \"module\";") {
			auto& node = parse_import("import { foo, bar } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 2);

			auto& foo = node.named_specifiers()[0];
			REQUIRE(foo.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(foo.name->to_string() == "foo");
			REQUIRE_FALSE(foo.alias.has_value());
			REQUIRE_FALSE(foo.type_keyword.has_value());

			auto& bar = node.named_specifiers()[1];
			REQUIRE(bar.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(bar.name->to_string() == "bar");
			REQUIRE_FALSE(bar.alias.has_value());
			REQUIRE_FALSE(bar.type_keyword.has_value());

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import { foo as bar } from \"module\";") {
			auto& node = parse_import("import { foo as bar } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 1);
			auto& spec = node.named_specifiers()[0];
			REQUIRE_FALSE(spec.type_keyword.has_value());
			REQUIRE(spec.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name->to_string() == "foo");
			REQUIRE(spec.alias.has_value());
			REQUIRE(spec.alias->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*spec.alias)->to_string() == "bar");

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import { default as foo } from \"module\";") {
			auto& node =
				parse_import("import { default as foo } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 1);
			auto& spec = node.named_specifiers()[0];
			REQUIRE_FALSE(spec.type_keyword.has_value());
			REQUIRE(spec.name.is<tscc::lex::tokens::default_token>());
			REQUIRE(spec.alias.has_value());
			REQUIRE(spec.alias->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*spec.alias)->to_string() == "foo");

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("empty named imports: import { } from \"module\";") {
			auto& node = parse_import("import { } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 0);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("trailing comma: import { foo, } from \"module\";") {
			auto& node = parse_import("import { foo, } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 1);
			REQUIRE(node.named_specifiers()[0]
						.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name->to_string() == "foo");
			REQUIRE_FALSE(node.named_specifiers()[0].alias.has_value());
			REQUIRE_FALSE(node.named_specifiers()[0].type_keyword.has_value());

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("multiple specifiers with aliases") {
			auto& node = parse_import(
				"import { foo, bar as baz, qux } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 3);

			REQUIRE(node.named_specifiers()[0]
						.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name->to_string() == "foo");
			REQUIRE_FALSE(node.named_specifiers()[0].alias.has_value());
			REQUIRE_FALSE(node.named_specifiers()[0].type_keyword.has_value());

			REQUIRE(node.named_specifiers()[1]
						.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[1].name->to_string() == "bar");
			REQUIRE(node.named_specifiers()[1].alias.has_value());
			REQUIRE(node.named_specifiers()[1]
						.alias->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.named_specifiers()[1].alias)->to_string() == "baz");
			REQUIRE_FALSE(node.named_specifiers()[1].type_keyword.has_value());

			REQUIRE(node.named_specifiers()[2]
						.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[2].name->to_string() == "qux");
			REQUIRE_FALSE(node.named_specifiers()[2].alias.has_value());
			REQUIRE_FALSE(node.named_specifiers()[2].type_keyword.has_value());

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("default with secondary bindings") {
		SECTION("import foo, { bar } from \"module\";") {
			auto& node = parse_import("import foo, { bar } from \"module\";");

			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "foo");

			REQUIRE(node.named_specifiers().size() == 1);
			REQUIRE(node.named_specifiers()[0]
						.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name->to_string() == "bar");
			REQUIRE_FALSE(node.named_specifiers()[0].alias.has_value());
			REQUIRE_FALSE(node.named_specifiers()[0].type_keyword.has_value());

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import foo, * as ns from \"module\";") {
			auto& node = parse_import("import foo, * as ns from \"module\";");

			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "foo");
			REQUIRE(node.namespace_name() != nullptr);
			REQUIRE(node.namespace_name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.namespace_name())->to_string() == "ns");

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("type-only modifier") {
		SECTION("import type foo from \"module\";") {
			auto& node = parse_import("import type foo from \"module\";");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "foo");

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import type * as ns from \"module\";") {
			auto& node = parse_import("import type * as ns from \"module\";");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.namespace_name() != nullptr);
			REQUIRE(node.namespace_name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.namespace_name())->to_string() == "ns");

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import type { foo } from \"module\";") {
			auto& node = parse_import("import type { foo } from \"module\";");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());

			REQUIRE(node.named_specifiers().size() == 1);
			REQUIRE(node.named_specifiers()[0]
						.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name->to_string() == "foo");
			REQUIRE_FALSE(node.named_specifiers()[0].alias.has_value());
			REQUIRE_FALSE(node.named_specifiers()[0].type_keyword.has_value());

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import type { foo, bar as baz } from \"module\";") {
			auto& node = parse_import(
				"import type { foo, bar as baz } from \"module\";");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());

			REQUIRE(node.named_specifiers().size() == 2);

			REQUIRE(node.named_specifiers()[0]
						.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name->to_string() == "foo");
			REQUIRE_FALSE(node.named_specifiers()[0].alias.has_value());
			REQUIRE_FALSE(node.named_specifiers()[0].type_keyword.has_value());

			REQUIRE(node.named_specifiers()[1]
						.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[1].name->to_string() == "bar");
			REQUIRE(node.named_specifiers()[1].alias.has_value());
			REQUIRE(node.named_specifiers()[1]
						.alias->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.named_specifiers()[1].alias)->to_string() == "baz");
			REQUIRE_FALSE(node.named_specifiers()[1].type_keyword.has_value());

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("inline type specifiers") {
		SECTION("import { type foo } from \"module\";") {
			auto& node = parse_import("import { type foo } from \"module\";");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.named_specifiers().size() == 1);

			auto& spec = node.named_specifiers()[0];
			REQUIRE(spec.type_keyword.has_value());
			REQUIRE(spec.type_keyword->is<tscc::lex::tokens::type_token>());
			REQUIRE(spec.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name->to_string() == "foo");
			REQUIRE_FALSE(spec.alias.has_value());

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import { type foo as bar } from \"module\";") {
			auto& node =
				parse_import("import { type foo as bar } from \"module\";");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.named_specifiers().size() == 1);

			auto& spec = node.named_specifiers()[0];
			REQUIRE(spec.type_keyword.has_value());
			REQUIRE(spec.type_keyword->is<tscc::lex::tokens::type_token>());
			REQUIRE(spec.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name->to_string() == "foo");
			REQUIRE(spec.alias.has_value());
			REQUIRE(spec.alias->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*spec.alias)->to_string() == "bar");

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import { type foo, bar } from \"module\";") {
			auto& node =
				parse_import("import { type foo, bar } from \"module\";");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.named_specifiers().size() == 2);

			auto& typed = node.named_specifiers()[0];
			REQUIRE(typed.type_keyword.has_value());
			REQUIRE(typed.type_keyword->is<tscc::lex::tokens::type_token>());
			REQUIRE(typed.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(typed.name->to_string() == "foo");
			REQUIRE_FALSE(typed.alias.has_value());

			auto& untyped = node.named_specifiers()[1];
			REQUIRE_FALSE(untyped.type_keyword.has_value());
			REQUIRE(untyped.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(untyped.name->to_string() == "bar");
			REQUIRE_FALSE(untyped.alias.has_value());

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("type as identifier") {
		SECTION("import type from \"module\"; — type is the binding name") {
			auto& node = parse_import("import type from \"module\";");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "type");

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION(
			"import type, { foo } from \"module\"; — type is binding name") {
			auto& node = parse_import("import type, { foo } from \"module\";");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "type");

			REQUIRE(node.named_specifiers().size() == 1);
			REQUIRE(node.named_specifiers()[0]
						.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name->to_string() == "foo");
			REQUIRE_FALSE(node.named_specifiers()[0].alias.has_value());
			REQUIRE_FALSE(node.named_specifiers()[0].type_keyword.has_value());

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import { type } from \"module\"; — type is specifier name") {
			auto& node = parse_import("import { type } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 1);
			auto& spec = node.named_specifiers()[0];
			REQUIRE_FALSE(spec.type_keyword.has_value());
			REQUIRE(spec.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name->to_string() == "type");
			REQUIRE_FALSE(spec.alias.has_value());

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION(
			"import { type as t } from \"module\"; — type is specifier name") {
			auto& node = parse_import("import { type as t } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 1);
			auto& spec = node.named_specifiers()[0];
			REQUIRE_FALSE(spec.type_keyword.has_value());
			REQUIRE(spec.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name->to_string() == "type");
			REQUIRE(spec.alias.has_value());
			REQUIRE(spec.alias->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*spec.alias)->to_string() == "t");

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION(
			"import { type, foo } from \"module\"; — type is specifier name") {
			auto& node = parse_import("import { type, foo } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 2);

			REQUIRE_FALSE(node.named_specifiers()[0].type_keyword.has_value());
			REQUIRE(node.named_specifiers()[0]
						.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name->to_string() == "type");
			REQUIRE_FALSE(node.named_specifiers()[0].alias.has_value());

			REQUIRE_FALSE(node.named_specifiers()[1].type_keyword.has_value());
			REQUIRE(node.named_specifiers()[1]
						.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[1].name->to_string() == "foo");
			REQUIRE_FALSE(node.named_specifiers()[1].alias.has_value());

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION(
			"import type type from \"module\"; — first is modifier, second is "
			"binding") {
			auto& node = parse_import("import type type from \"module\";");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "type");

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("import equals require") {
		SECTION("import foo = require(\"module\");") {
			auto& node = parse_import("import foo = require(\"module\");");

			REQUIRE(node.equals_name() != nullptr);
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.equals_name())->to_string() == "foo");
			REQUIRE(node.require_module_specifier() != nullptr);
			REQUIRE(node.require_module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.require_module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.module_specifier() == nullptr);
			REQUIRE(node.entity_identifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
			REQUIRE(node.attributes().empty());
		}

		SECTION("import type foo = require(\"module\");") {
			auto& node = parse_import("import type foo = require(\"module\");");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.equals_name() != nullptr);
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.equals_name())->to_string() == "foo");
			REQUIRE(node.require_module_specifier() != nullptr);
			REQUIRE(node.require_module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.require_module_specifier()) == U"module");

			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.module_specifier() == nullptr);
			REQUIRE(node.entity_identifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("import equals entity name") {
		SECTION("import foo = Namespace.Thing;") {
			auto& node = parse_import("import foo = Namespace.Thing;");

			REQUIRE(node.equals_name() != nullptr);
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.equals_name())->to_string() == "foo");
			REQUIRE(node.entity_identifiers().size() == 2);
			REQUIRE(node.entity_identifiers()[0]
						.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_identifiers()[0]->to_string() == "Namespace");
			REQUIRE(node.entity_identifiers()[1]
						.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_identifiers()[1]->to_string() == "Thing");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.require_module_specifier() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.module_specifier() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import foo = A.B.C;") {
			auto& node = parse_import("import foo = A.B.C;");

			REQUIRE(node.equals_name() != nullptr);
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.equals_name())->to_string() == "foo");
			REQUIRE(node.entity_identifiers().size() == 3);
			REQUIRE(node.entity_identifiers()[0]
						.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_identifiers()[0]->to_string() == "A");
			REQUIRE(node.entity_identifiers()[1]
						.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_identifiers()[1]->to_string() == "B");
			REQUIRE(node.entity_identifiers()[2]
						.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_identifiers()[2]->to_string() == "C");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.require_module_specifier() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.module_specifier() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import type foo = Namespace.Thing;") {
			auto& node = parse_import("import type foo = Namespace.Thing;");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.equals_name() != nullptr);
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.equals_name())->to_string() == "foo");
			REQUIRE(node.entity_identifiers().size() == 2);
			REQUIRE(node.entity_identifiers()[0]
						.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_identifiers()[0]->to_string() == "Namespace");
			REQUIRE(node.entity_identifiers()[1]
						.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_identifiers()[1]->to_string() == "Thing");

			REQUIRE(node.require_module_specifier() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.module_specifier() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import type = require(\"module\"); — type as equals name") {
			auto& node = parse_import("import type = require(\"module\");");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.equals_name() != nullptr);
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.equals_name())->to_string() == "type");
			REQUIRE(node.require_module_specifier() != nullptr);
			REQUIRE(node.require_module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.require_module_specifier()) == U"module");

			REQUIRE(node.entity_identifiers().empty());
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.module_specifier() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("import attributes") {
		SECTION("import foo from \"m\" with { type: \"json\" };") {
			auto& node =
				parse_import("import foo from \"m\" with { type: \"json\" };");

			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "foo");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"m");
			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(
				node.attributes_keyword()->is<tscc::lex::tokens::with_token>());
			REQUIRE(node.attributes().size() == 1);

			auto& attr = node.attributes()[0];
			REQUIRE(attr.key.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(attr.key->to_string() == "type");
			REQUIRE(attr.value.is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(attr.value) == U"json");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
		}

		SECTION("import foo from \"m\" assert { type: \"json\" };") {
			auto& node = parse_import(
				"import foo from \"m\" assert { type: \"json\" };");

			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "foo");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"m");

			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(node.attributes_keyword()
						->is<tscc::lex::tokens::assert_token>());
			REQUIRE(node.attributes().size() == 1);
			REQUIRE(node.attributes()[0]
						.key.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.attributes()[0].key->to_string() == "type");
			REQUIRE(node.attributes()[0]
						.value.is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.attributes()[0].value) == U"json");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
		}

		SECTION(
			"side-effect with attributes: import \"m\" with { type: \"json\" "
			"};") {
			auto& node = parse_import("import \"m\" with { type: \"json\" };");

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"m");
			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(
				node.attributes_keyword()->is<tscc::lex::tokens::with_token>());
			REQUIRE(node.attributes().size() == 1);
			REQUIRE(node.attributes()[0]
						.key.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.attributes()[0].key->to_string() == "type");
			REQUIRE(node.attributes()[0]
						.value.is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.attributes()[0].value) == U"json");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
		}

		SECTION("multiple attributes") {
			auto& node = parse_import(
				"import foo from \"m\" with { type: \"json\", integrity: "
				"\"sha\" };");

			REQUIRE(node.default_binding() != nullptr);
			REQUIRE((*node.default_binding())->to_string() == "foo");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(string_value(node.module_specifier()) == U"m");

			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(
				node.attributes_keyword()->is<tscc::lex::tokens::with_token>());
			REQUIRE(node.attributes().size() == 2);

			REQUIRE(node.attributes()[0]
						.key.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.attributes()[0].key->to_string() == "type");
			REQUIRE(node.attributes()[0]
						.value.is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.attributes()[0].value) == U"json");

			REQUIRE(node.attributes()[1]
						.key.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.attributes()[1].key->to_string() == "integrity");
			REQUIRE(node.attributes()[1]
						.value.is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.attributes()[1].value) == U"sha");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
		}

		SECTION("empty attributes") {
			auto& node = parse_import("import foo from \"m\" with { };");

			REQUIRE(node.default_binding() != nullptr);
			REQUIRE((*node.default_binding())->to_string() == "foo");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(string_value(node.module_specifier()) == U"m");
			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(
				node.attributes_keyword()->is<tscc::lex::tokens::with_token>());
			REQUIRE(node.attributes().empty());

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
		}

		SECTION("trailing comma in attributes") {
			auto& node =
				parse_import("import foo from \"m\" with { type: \"json\", };");

			REQUIRE(node.default_binding() != nullptr);
			REQUIRE((*node.default_binding())->to_string() == "foo");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(string_value(node.module_specifier()) == U"m");
			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(node.attributes().size() == 1);
			REQUIRE(node.attributes()[0].key->to_string() == "type");
			REQUIRE(string_value(node.attributes()[0].value) == U"json");
		}
	}

	SECTION("without semicolon") {
		SECTION("import \"module\" at EOF") {
			auto& node = parse_import("import \"module\"");

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import foo from \"module\" at EOF") {
			auto& node = parse_import("import foo from \"module\"");

			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "foo");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.equals_name() == nullptr);
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import foo = require(\"module\") at EOF") {
			auto& node = parse_import("import foo = require(\"module\")");

			REQUIRE(node.equals_name() != nullptr);
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.equals_name())->to_string() == "foo");
			REQUIRE(node.require_module_specifier() != nullptr);
			REQUIRE(node.require_module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.require_module_specifier()) == U"module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.module_specifier() == nullptr);
			REQUIRE(node.entity_identifiers().empty());
		}

		SECTION("import foo = A.B at EOF") {
			auto& node = parse_import("import foo = A.B");

			REQUIRE(node.equals_name() != nullptr);
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.equals_name())->to_string() == "foo");
			REQUIRE(node.entity_identifiers().size() == 2);
			REQUIRE(node.entity_identifiers()[0]
						.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_identifiers()[0]->to_string() == "A");
			REQUIRE(node.entity_identifiers()[1]
						.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_identifiers()[1]->to_string() == "B");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.module_specifier() == nullptr);
			REQUIRE(node.require_module_specifier() == nullptr);
		}
	}

	SECTION("newline handling") {
		SECTION("newline before module specifier") {
			auto& node = parse_import("import\n\"module\";");

			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
		}

		SECTION("newline between from and module specifier") {
			auto& node = parse_import("import foo from\n\"module\";");

			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "foo");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
		}

		SECTION("newline inside named imports") {
			auto& node =
				parse_import("import {\nfoo,\nbar\n} from \"module\";");

			REQUIRE(node.named_specifiers().size() == 2);
			REQUIRE(node.named_specifiers()[0]
						.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name->to_string() == "foo");
			REQUIRE(node.named_specifiers()[1]
						.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[1].name->to_string() == "bar");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(string_value(node.module_specifier()) == U"module");
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.namespace_name() == nullptr);
		}

		SECTION("newline before from keyword") {
			auto& node = parse_import("import foo\nfrom \"module\";");

			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "foo");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");
			REQUIRE(node.namespace_name() == nullptr);
			REQUIRE(node.named_specifiers().empty());
		}

		SECTION("newline before as in namespace import") {
			auto& node = parse_import("import *\nas ns from \"module\";");

			REQUIRE(node.namespace_name() != nullptr);
			REQUIRE(node.namespace_name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.namespace_name())->to_string() == "ns");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.module_specifier()) == U"module");
			REQUIRE(node.default_binding() == nullptr);
			REQUIRE(node.named_specifiers().empty());
		}

		SECTION("newline before with — continues as attributes") {
			auto& node =
				parse_import("import foo from \"m\"\nwith { type: \"json\" };");

			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "foo");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(string_value(node.module_specifier()) == U"m");
			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(
				node.attributes_keyword()->is<tscc::lex::tokens::with_token>());
			REQUIRE(node.attributes().size() == 1);
			REQUIRE(node.attributes()[0].key->to_string() == "type");
			REQUIRE(string_value(node.attributes()[0].value) == U"json");
		}

		SECTION("newline before assert — continues as attributes") {
			auto& node = parse_import(
				"import foo from \"m\"\nassert { type: \"json\" };");

			REQUIRE(node.default_binding() != nullptr);
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.default_binding())->to_string() == "foo");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(string_value(node.module_specifier()) == U"m");
			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(node.attributes_keyword()
						->is<tscc::lex::tokens::assert_token>());
			REQUIRE(node.attributes().size() == 1);
			REQUIRE(node.attributes()[0].key->to_string() == "type");
			REQUIRE(string_value(node.attributes()[0].value) == U"json");
		}

		SECTION("ASI: two imports separated by newline") {
			test_utils::parse_result r;
			r.stream = std::make_unique<std::stringstream>(
				"import \"a\"\nimport \"b\"");
			r.source = std::make_shared<fake_source>("test.ts");
			r.lexer = std::make_unique<tscc::lex::lexer>(*r.stream, r.source);
			r.parser = std::make_unique<tscc::parse::parser>(*r.lexer);

			auto it = r.parser->begin();
			REQUIRE(it != r.parser->end());
			auto first = std::move(*it);
			REQUIRE(first != nullptr);
			auto* first_import =
				dynamic_cast<tscc::parse::ast::import_node*>(first.get());
			REQUIRE(first_import != nullptr);
			REQUIRE(first_import->module_specifier() != nullptr);
			REQUIRE(string_value(first_import->module_specifier()) == U"a");
			REQUIRE(first_import->default_binding() == nullptr);

			++it;
			REQUIRE(it != r.parser->end());
			auto second = std::move(*it);
			REQUIRE(second != nullptr);
			auto* second_import =
				dynamic_cast<tscc::parse::ast::import_node*>(second.get());
			REQUIRE(second_import != nullptr);
			REQUIRE(second_import->module_specifier() != nullptr);
			REQUIRE(string_value(second_import->module_specifier()) == U"b");
			REQUIRE(second_import->default_binding() == nullptr);
		}

		SECTION("ASI: import with bindings followed by another import") {
			test_utils::parse_result r;
			r.stream = std::make_unique<std::stringstream>(
				"import foo from \"a\"\nimport bar from \"b\"");
			r.source = std::make_shared<fake_source>("test.ts");
			r.lexer = std::make_unique<tscc::lex::lexer>(*r.stream, r.source);
			r.parser = std::make_unique<tscc::parse::parser>(*r.lexer);

			auto it = r.parser->begin();
			REQUIRE(it != r.parser->end());
			auto first = std::move(*it);
			auto* first_import =
				dynamic_cast<tscc::parse::ast::import_node*>(first.get());
			REQUIRE(first_import != nullptr);
			REQUIRE(first_import->default_binding() != nullptr);
			REQUIRE(
				first_import->default_binding()
					->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(
				(*first_import->default_binding())->to_string() == "foo");
			REQUIRE(first_import->module_specifier() != nullptr);
			REQUIRE(string_value(first_import->module_specifier()) == U"a");

			++it;
			REQUIRE(it != r.parser->end());
			auto second = std::move(*it);
			auto* second_import =
				dynamic_cast<tscc::parse::ast::import_node*>(second.get());
			REQUIRE(second_import != nullptr);
			REQUIRE(second_import->default_binding() != nullptr);
			REQUIRE(
				second_import->default_binding()
					->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(
				(*second_import->default_binding())->to_string() == "bar");
			REQUIRE(second_import->module_specifier() != nullptr);
			REQUIRE(string_value(second_import->module_specifier()) == U"b");
		}
	}

	SECTION("source locations") {
		SECTION("location points to import keyword") {
			auto& node = parse_import("import \"module\";");

			REQUIRE(node.location().line() == 0);
			REQUIRE(node.location().column() == 0);
		}

		SECTION("import on second line") {
			auto& node = parse_import("\nimport \"module\";");

			REQUIRE(node.location().line() == 1);
			REQUIRE(node.location().column() == 0);
		}
	}

	SECTION("parse errors") {
		SECTION("import followed by unexpected token") {
			REQUIRE_THROWS_AS(parse_import("import +"),
							  tscc::parse::expected_token);
		}

		SECTION("import * without as") {
			REQUIRE_THROWS_AS(parse_import("import * from \"m\";"),
							  tscc::parse::expected_token);
		}

		SECTION("import * as without identifier") {
			REQUIRE_THROWS_AS(parse_import("import * as ;"),
							  tscc::parse::expected_token);
		}

		SECTION("import foo without from or =") {
			REQUIRE_THROWS_AS(parse_import("import foo +"),
							  tscc::parse::expected_token);
		}

		SECTION("import { foo without closing brace") {
			REQUIRE_THROWS_AS(parse_import("import { foo"),
							  tscc::parse::unexpected_end_of_text);
		}

		SECTION("import { foo as without alias") {
			REQUIRE_THROWS_AS(parse_import("import { foo as }"),
							  tscc::parse::expected_token);
		}

		SECTION("import foo from without string") {
			REQUIRE_THROWS_AS(parse_import("import foo from ;"),
							  tscc::parse::expected_token);
		}

		SECTION("import foo = require( without string") {
			REQUIRE_THROWS_AS(parse_import("import foo = require(;"),
							  tscc::parse::expected_token);
		}

		SECTION("import foo = require(\"m\" without close paren") {
			REQUIRE_THROWS_AS(parse_import("import foo = require(\"m\" ;"),
							  tscc::parse::expected_token);
		}

		SECTION("attributes without opening brace") {
			REQUIRE_THROWS_AS(parse_import("import foo from \"m\" with ;"),
							  tscc::parse::expected_token);
		}

		SECTION("attribute missing colon") {
			REQUIRE_THROWS_AS(
				parse_import("import foo from \"m\" with { type \"json\" }"),
				tscc::parse::expected_token);
		}

		SECTION("attribute missing value") {
			REQUIRE_THROWS_AS(
				parse_import("import foo from \"m\" with { type: }"),
				tscc::parse::expected_token);
		}
	}

	SECTION("contextual keyword normalization") {
		SECTION("namespace name from contextual keyword") {
			auto& node = parse_import("import * as from from \"module\";");

			REQUIRE(node.namespace_name() != nullptr);
			REQUIRE(node.namespace_name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*node.namespace_name())->to_string() == "from");
			REQUIRE(node.module_specifier() != nullptr);
			REQUIRE(string_value(node.module_specifier()) == U"module");
		}

		SECTION("entity identifiers normalized") {
			auto& node = parse_import("import foo = type.from.as;");

			REQUIRE(node.entity_identifiers().size() == 3);
			for (auto& id : node.entity_identifiers()) {
				REQUIRE(id.is<tscc::lex::tokens::identifier_token>());
			}
			REQUIRE(node.entity_identifiers()[0]->to_string() == "type");
			REQUIRE(node.entity_identifiers()[1]->to_string() == "from");
			REQUIRE(node.entity_identifiers()[2]->to_string() == "as");

			REQUIRE((*node.equals_name())->to_string() == "foo");
		}

		SECTION("attribute key normalized") {
			auto& node = parse_import(
				"import foo from \"m\" with { assert: \"json\" };");

			REQUIRE(node.attributes().size() == 1);
			REQUIRE(node.attributes()[0]
						.key.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.attributes()[0].key->to_string() == "assert");
			REQUIRE(node.attributes()[0]
						.value.is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(string_value(node.attributes()[0].value) == U"json");

			REQUIRE((*node.default_binding())->to_string() == "foo");
			REQUIRE(string_value(node.module_specifier()) == U"m");
		}

		SECTION("specifier alias normalized") {
			auto& node =
				parse_import("import { foo as type } from \"module\";");

			auto& spec = node.named_specifiers()[0];
			REQUIRE(spec.name.is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name->to_string() == "foo");
			REQUIRE(spec.alias.has_value());
			REQUIRE(spec.alias->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*spec.alias)->to_string() == "type");

			REQUIRE(string_value(node.module_specifier()) == U"module");
		}

		SECTION("default_token not normalized in specifier name") {
			auto& node =
				parse_import("import { default as foo } from \"module\";");

			auto& spec = node.named_specifiers()[0];
			REQUIRE(spec.name.is<tscc::lex::tokens::default_token>());
			REQUIRE(spec.alias.has_value());
			REQUIRE(spec.alias->is<tscc::lex::tokens::identifier_token>());
			REQUIRE((*spec.alias)->to_string() == "foo");

			REQUIRE(string_value(node.module_specifier()) == U"module");
		}
	}
}
