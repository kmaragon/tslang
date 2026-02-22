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

TEST_CASE("import", "[parser][import]") {
	SECTION("side-effect") {
		SECTION("import \"module\";") {
			auto& node = parse_import("import \"module\";");

			REQUIRE(
				node.import_keyword()->is<tscc::lex::tokens::import_token>());
			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE_FALSE(node.require_module_specifier());
			REQUIRE(node.entity_name().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
			REQUIRE(node.attributes().empty());
		}

		SECTION("import 'module';") {
			auto& node = parse_import("import 'module';");

			REQUIRE(
				node.import_keyword()->is<tscc::lex::tokens::import_token>());
			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE_FALSE(node.require_module_specifier());
			REQUIRE(node.entity_name().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
			REQUIRE(node.attributes().empty());
		}
	}

	SECTION("default binding") {
		SECTION("import foo from \"module\";") {
			auto& node = parse_import("import foo from \"module\";");

			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "foo");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE_FALSE(node.require_module_specifier());
			REQUIRE(node.entity_name().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
			REQUIRE(node.attributes().empty());
		}
	}

	SECTION("namespace binding") {
		SECTION("import * as ns from \"module\";") {
			auto& node = parse_import("import * as ns from \"module\";");

			REQUIRE(node.namespace_name());
			REQUIRE(node.namespace_name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.namespace_name().value() == "ns");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE_FALSE(node.require_module_specifier());
			REQUIRE(node.entity_name().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
			REQUIRE(node.attributes().empty());
		}
	}

	SECTION("named bindings") {
		SECTION("import { foo } from \"module\";") {
			auto& node = parse_import("import { foo } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 1);

			auto& spec = node.named_specifiers()[0];
			REQUIRE(spec.type_keyword() == nullptr);
			REQUIRE(spec.name());
			REQUIRE(spec.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name().value() == "foo");
			REQUIRE_FALSE(spec.alias());

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE_FALSE(node.require_module_specifier());
			REQUIRE(node.entity_name().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
			REQUIRE(node.attributes().empty());
		}

		SECTION("import { foo, bar } from \"module\";") {
			auto& node = parse_import("import { foo, bar } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 2);

			auto& foo = node.named_specifiers()[0];
			REQUIRE(foo.name());
			REQUIRE(foo.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(foo.name().value() == "foo");
			REQUIRE_FALSE(foo.alias());
			REQUIRE(foo.type_keyword() == nullptr);

			auto& bar = node.named_specifiers()[1];
			REQUIRE(bar.name());
			REQUIRE(bar.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(bar.name().value() == "bar");
			REQUIRE_FALSE(bar.alias());
			REQUIRE(bar.type_keyword() == nullptr);

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import { foo as bar } from \"module\";") {
			auto& node = parse_import("import { foo as bar } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 1);
			auto& spec = node.named_specifiers()[0];
			REQUIRE(spec.type_keyword() == nullptr);
			REQUIRE(spec.name());
			REQUIRE(spec.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name().value() == "foo");
			REQUIRE(spec.alias());
			REQUIRE(spec.alias()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.alias().value() == "bar");

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import { default as foo } from \"module\";") {
			auto& node =
				parse_import("import { default as foo } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 1);
			auto& spec = node.named_specifiers()[0];
			REQUIRE(spec.type_keyword() == nullptr);
			REQUIRE(spec.is_default());
			REQUIRE(spec.alias());
			REQUIRE(spec.alias()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.alias().value() == "foo");

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("empty named imports: import { } from \"module\";") {
			auto& node = parse_import("import { } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 0);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("trailing comma: import { foo, } from \"module\";") {
			auto& node = parse_import("import { foo, } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 1);
			REQUIRE(node.named_specifiers()[0].name());
			REQUIRE(node.named_specifiers()[0]
						.name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name().value() == "foo");
			REQUIRE_FALSE(node.named_specifiers()[0].alias());
			REQUIRE(node.named_specifiers()[0].type_keyword() == nullptr);

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("multiple specifiers with aliases") {
			auto& node = parse_import(
				"import { foo, bar as baz, qux } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 3);

			REQUIRE(node.named_specifiers()[0].name());
			REQUIRE(node.named_specifiers()[0]
						.name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name().value() == "foo");
			REQUIRE_FALSE(node.named_specifiers()[0].alias());
			REQUIRE(node.named_specifiers()[0].type_keyword() == nullptr);

			REQUIRE(node.named_specifiers()[1].name());
			REQUIRE(node.named_specifiers()[1]
						.name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[1].name().value() == "bar");
			REQUIRE(node.named_specifiers()[1].alias());
			REQUIRE(node.named_specifiers()[1]
						.alias()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[1].alias().value() == "baz");
			REQUIRE(node.named_specifiers()[1].type_keyword() == nullptr);

			REQUIRE(node.named_specifiers()[2].name());
			REQUIRE(node.named_specifiers()[2]
						.name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[2].name().value() == "qux");
			REQUIRE_FALSE(node.named_specifiers()[2].alias());
			REQUIRE(node.named_specifiers()[2].type_keyword() == nullptr);

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("default with secondary bindings") {
		SECTION("import foo, { bar } from \"module\";") {
			auto& node = parse_import("import foo, { bar } from \"module\";");

			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "foo");

			REQUIRE(node.named_specifiers().size() == 1);
			REQUIRE(node.named_specifiers()[0].name());
			REQUIRE(node.named_specifiers()[0]
						.name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name().value() == "bar");
			REQUIRE_FALSE(node.named_specifiers()[0].alias());
			REQUIRE(node.named_specifiers()[0].type_keyword() == nullptr);

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import foo, * as ns from \"module\";") {
			auto& node = parse_import("import foo, * as ns from \"module\";");

			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "foo");
			REQUIRE(node.namespace_name());
			REQUIRE(node.namespace_name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.namespace_name().value() == "ns");

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("type-only modifier") {
		SECTION("import type foo from \"module\";") {
			auto& node = parse_import("import type foo from \"module\";");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "foo");

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import type * as ns from \"module\";") {
			auto& node = parse_import("import type * as ns from \"module\";");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.namespace_name());
			REQUIRE(node.namespace_name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.namespace_name().value() == "ns");

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE_FALSE(node.default_binding());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import type { foo } from \"module\";") {
			auto& node = parse_import("import type { foo } from \"module\";");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());

			REQUIRE(node.named_specifiers().size() == 1);
			REQUIRE(node.named_specifiers()[0].name());
			REQUIRE(node.named_specifiers()[0]
						.name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name().value() == "foo");
			REQUIRE_FALSE(node.named_specifiers()[0].alias());
			REQUIRE(node.named_specifiers()[0].type_keyword() == nullptr);

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import type { foo, bar as baz } from \"module\";") {
			auto& node = parse_import(
				"import type { foo, bar as baz } from \"module\";");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());

			REQUIRE(node.named_specifiers().size() == 2);

			REQUIRE(node.named_specifiers()[0].name());
			REQUIRE(node.named_specifiers()[0]
						.name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name().value() == "foo");
			REQUIRE_FALSE(node.named_specifiers()[0].alias());
			REQUIRE(node.named_specifiers()[0].type_keyword() == nullptr);

			REQUIRE(node.named_specifiers()[1].name());
			REQUIRE(node.named_specifiers()[1]
						.name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[1].name().value() == "bar");
			REQUIRE(node.named_specifiers()[1].alias());
			REQUIRE(node.named_specifiers()[1]
						.alias()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[1].alias().value() == "baz");
			REQUIRE(node.named_specifiers()[1].type_keyword() == nullptr);

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("inline type specifiers") {
		SECTION("import { type foo } from \"module\";") {
			auto& node = parse_import("import { type foo } from \"module\";");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.named_specifiers().size() == 1);

			auto& spec = node.named_specifiers()[0];
			REQUIRE(spec.type_keyword() != nullptr);
			REQUIRE(
				spec.type_keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(spec.name());
			REQUIRE(spec.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name().value() == "foo");
			REQUIRE_FALSE(spec.alias());

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import { type foo as bar } from \"module\";") {
			auto& node =
				parse_import("import { type foo as bar } from \"module\";");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.named_specifiers().size() == 1);

			auto& spec = node.named_specifiers()[0];
			REQUIRE(spec.type_keyword() != nullptr);
			REQUIRE(
				spec.type_keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(spec.name());
			REQUIRE(spec.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name().value() == "foo");
			REQUIRE(spec.alias());
			REQUIRE(spec.alias()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.alias().value() == "bar");

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import { type foo, bar } from \"module\";") {
			auto& node =
				parse_import("import { type foo, bar } from \"module\";");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.named_specifiers().size() == 2);

			auto& typed = node.named_specifiers()[0];
			REQUIRE(typed.type_keyword() != nullptr);
			REQUIRE(
				typed.type_keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(typed.name());
			REQUIRE(typed.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(typed.name().value() == "foo");
			REQUIRE_FALSE(typed.alias());

			auto& untyped = node.named_specifiers()[1];
			REQUIRE(untyped.type_keyword() == nullptr);
			REQUIRE(untyped.name());
			REQUIRE(
				untyped.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(untyped.name().value() == "bar");
			REQUIRE_FALSE(untyped.alias());

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("type as identifier") {
		SECTION("import type from \"module\"; — type is the binding name") {
			auto& node = parse_import("import type from \"module\";");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "type");

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION(
			"import type, { foo } from \"module\"; — type is binding name") {
			auto& node = parse_import("import type, { foo } from \"module\";");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "type");

			REQUIRE(node.named_specifiers().size() == 1);
			REQUIRE(node.named_specifiers()[0].name());
			REQUIRE(node.named_specifiers()[0]
						.name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name().value() == "foo");
			REQUIRE_FALSE(node.named_specifiers()[0].alias());
			REQUIRE(node.named_specifiers()[0].type_keyword() == nullptr);

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import { type } from \"module\"; — type is specifier name") {
			auto& node = parse_import("import { type } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 1);
			auto& spec = node.named_specifiers()[0];
			REQUIRE(spec.type_keyword() == nullptr);
			REQUIRE(spec.name());
			REQUIRE(spec.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name().value() == "type");
			REQUIRE_FALSE(spec.alias());

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION(
			"import { type as t } from \"module\"; — type is specifier name") {
			auto& node = parse_import("import { type as t } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 1);
			auto& spec = node.named_specifiers()[0];
			REQUIRE(spec.type_keyword() == nullptr);
			REQUIRE(spec.name());
			REQUIRE(spec.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name().value() == "type");
			REQUIRE(spec.alias());
			REQUIRE(spec.alias()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.alias().value() == "t");

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION(
			"import { type, foo } from \"module\"; — type is specifier name") {
			auto& node = parse_import("import { type, foo } from \"module\";");

			REQUIRE(node.named_specifiers().size() == 2);

			REQUIRE(node.named_specifiers()[0].type_keyword() == nullptr);
			REQUIRE(node.named_specifiers()[0].name());
			REQUIRE(node.named_specifiers()[0]
						.name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name().value() == "type");
			REQUIRE_FALSE(node.named_specifiers()[0].alias());

			REQUIRE(node.named_specifiers()[1].type_keyword() == nullptr);
			REQUIRE(node.named_specifiers()[1].name());
			REQUIRE(node.named_specifiers()[1]
						.name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[1].name().value() == "foo");
			REQUIRE_FALSE(node.named_specifiers()[1].alias());

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION(
			"import type type from \"module\"; — first is modifier, second is "
			"binding") {
			auto& node = parse_import("import type type from \"module\";");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "type");

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("import equals require") {
		SECTION("import foo = require(\"module\");") {
			auto& node = parse_import("import foo = require(\"module\");");

			REQUIRE(node.equals_name());
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.equals_name().value() == "foo");
			REQUIRE(node.require_module_specifier());
			REQUIRE(node.require_module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.require_module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.module_specifier());
			REQUIRE(node.entity_name().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
			REQUIRE(node.attributes().empty());
		}

		SECTION("import type foo = require(\"module\");") {
			auto& node = parse_import("import type foo = require(\"module\");");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.equals_name());
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.equals_name().value() == "foo");
			REQUIRE(node.require_module_specifier());
			REQUIRE(node.require_module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.require_module_specifier().value() == "module");

			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.module_specifier());
			REQUIRE(node.entity_name().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("import equals entity name") {
		SECTION("import foo = Namespace.Thing;") {
			auto& node = parse_import("import foo = Namespace.Thing;");

			REQUIRE(node.equals_name());
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.equals_name().value() == "foo");
			REQUIRE(node.entity_name().size() == 2);
			REQUIRE(node.entity_name()[0]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_name()[0].value() == "Namespace");
			REQUIRE(node.entity_name()[1]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_name()[1].value() == "Thing");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.require_module_specifier());
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.module_specifier());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import foo = A.B.C;") {
			auto& node = parse_import("import foo = A.B.C;");

			REQUIRE(node.equals_name());
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.equals_name().value() == "foo");
			REQUIRE(node.entity_name().size() == 3);
			REQUIRE(node.entity_name()[0]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_name()[0].value() == "A");
			REQUIRE(node.entity_name()[1]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_name()[1].value() == "B");
			REQUIRE(node.entity_name()[2]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_name()[2].value() == "C");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.require_module_specifier());
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.module_specifier());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import type foo = Namespace.Thing;") {
			auto& node = parse_import("import type foo = Namespace.Thing;");

			REQUIRE(node.type_keyword() != nullptr);
			REQUIRE(node.type_keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.equals_name());
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.equals_name().value() == "foo");
			REQUIRE(node.entity_name().size() == 2);
			REQUIRE(node.entity_name()[0]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_name()[0].value() == "Namespace");
			REQUIRE(node.entity_name()[1]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_name()[1].value() == "Thing");

			REQUIRE_FALSE(node.require_module_specifier());
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.module_specifier());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import type = require(\"module\"); — type as equals name") {
			auto& node = parse_import("import type = require(\"module\");");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE(node.equals_name());
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.equals_name().value() == "type");
			REQUIRE(node.require_module_specifier());
			REQUIRE(node.require_module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.require_module_specifier().value() == "module");

			REQUIRE(node.entity_name().empty());
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.module_specifier());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE(node.attributes_keyword() == nullptr);
		}
	}

	SECTION("import attributes") {
		SECTION("import foo from \"m\" with { type: \"json\" };") {
			auto& node =
				parse_import("import foo from \"m\" with { type: \"json\" };");

			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "foo");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "m");
			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(
				node.attributes_keyword()->is<tscc::lex::tokens::with_token>());
			REQUIRE(node.attributes().size() == 1);

			auto& attr = node.attributes()[0];
			REQUIRE(attr.key());
			REQUIRE(attr.key()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(attr.key().value() == "type");
			REQUIRE(attr.value());
			REQUIRE(
				attr.value()->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(attr.value().value() == "json");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
		}

		SECTION("import foo from \"m\" assert { type: \"json\" };") {
			auto& node = parse_import(
				"import foo from \"m\" assert { type: \"json\" };");

			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "foo");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "m");

			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(node.attributes_keyword()
						->is<tscc::lex::tokens::assert_token>());
			REQUIRE(node.attributes().size() == 1);
			REQUIRE(node.attributes()[0].key());
			REQUIRE(node.attributes()[0]
						.key()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.attributes()[0].key().value() == "type");
			REQUIRE(node.attributes()[0].value());
			REQUIRE(node.attributes()[0]
						.value()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.attributes()[0].value().value() == "json");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
		}

		SECTION(
			"side-effect with attributes: import \"m\" with { type: \"json\" "
			"};") {
			auto& node = parse_import("import \"m\" with { type: \"json\" };");

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "m");
			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(
				node.attributes_keyword()->is<tscc::lex::tokens::with_token>());
			REQUIRE(node.attributes().size() == 1);
			REQUIRE(node.attributes()[0].key());
			REQUIRE(node.attributes()[0]
						.key()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.attributes()[0].key().value() == "type");
			REQUIRE(node.attributes()[0].value());
			REQUIRE(node.attributes()[0]
						.value()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.attributes()[0].value().value() == "json");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
		}

		SECTION("multiple attributes") {
			auto& node = parse_import(
				"import foo from \"m\" with { type: \"json\", integrity: "
				"\"sha\" };");

			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding().value() == "foo");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier().value() == "m");

			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(
				node.attributes_keyword()->is<tscc::lex::tokens::with_token>());
			REQUIRE(node.attributes().size() == 2);

			REQUIRE(node.attributes()[0].key());
			REQUIRE(node.attributes()[0]
						.key()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.attributes()[0].key().value() == "type");
			REQUIRE(node.attributes()[0].value());
			REQUIRE(node.attributes()[0]
						.value()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.attributes()[0].value().value() == "json");

			REQUIRE(node.attributes()[1].key());
			REQUIRE(node.attributes()[1]
						.key()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.attributes()[1].key().value() == "integrity");
			REQUIRE(node.attributes()[1].value());
			REQUIRE(node.attributes()[1]
						.value()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.attributes()[1].value().value() == "sha");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
		}

		SECTION("empty attributes") {
			auto& node = parse_import("import foo from \"m\" with { };");

			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding().value() == "foo");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier().value() == "m");
			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(
				node.attributes_keyword()->is<tscc::lex::tokens::with_token>());
			REQUIRE(node.attributes().empty());

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
		}

		SECTION("trailing comma in attributes") {
			auto& node =
				parse_import("import foo from \"m\" with { type: \"json\", };");

			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding().value() == "foo");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier().value() == "m");
			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(node.attributes().size() == 1);
			REQUIRE(node.attributes()[0].key().value() == "type");
			REQUIRE(node.attributes()[0].value().value() == "json");
		}
	}

	SECTION("without semicolon") {
		SECTION("import \"module\" at EOF") {
			auto& node = parse_import("import \"module\"");

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import foo from \"module\" at EOF") {
			auto& node = parse_import("import foo from \"module\"");

			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "foo");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
			REQUIRE_FALSE(node.equals_name());
			REQUIRE(node.attributes_keyword() == nullptr);
		}

		SECTION("import foo = require(\"module\") at EOF") {
			auto& node = parse_import("import foo = require(\"module\")");

			REQUIRE(node.equals_name());
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.equals_name().value() == "foo");
			REQUIRE(node.require_module_specifier());
			REQUIRE(node.require_module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.require_module_specifier().value() == "module");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.module_specifier());
			REQUIRE(node.entity_name().empty());
		}

		SECTION("import foo = A.B at EOF") {
			auto& node = parse_import("import foo = A.B");

			REQUIRE(node.equals_name());
			REQUIRE(
				node.equals_name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.equals_name().value() == "foo");
			REQUIRE(node.entity_name().size() == 2);
			REQUIRE(node.entity_name()[0]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_name()[0].value() == "A");
			REQUIRE(node.entity_name()[1]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.entity_name()[1].value() == "B");

			REQUIRE(node.type_keyword() == nullptr);
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.module_specifier());
			REQUIRE_FALSE(node.require_module_specifier());
		}
	}

	SECTION("newline handling") {
		SECTION("newline before module specifier") {
			auto& node = parse_import("import\n\"module\";");

			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
		}

		SECTION("newline between from and module specifier") {
			auto& node = parse_import("import foo from\n\"module\";");

			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "foo");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
		}

		SECTION("newline inside named imports") {
			auto& node =
				parse_import("import {\nfoo,\nbar\n} from \"module\";");

			REQUIRE(node.named_specifiers().size() == 2);
			REQUIRE(node.named_specifiers()[0].name());
			REQUIRE(node.named_specifiers()[0]
						.name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[0].name().value() == "foo");
			REQUIRE(node.named_specifiers()[1].name());
			REQUIRE(node.named_specifiers()[1]
						.name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.named_specifiers()[1].name().value() == "bar");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier().value() == "module");
			REQUIRE_FALSE(node.default_binding());
			REQUIRE_FALSE(node.namespace_name());
		}

		SECTION("newline before from keyword") {
			auto& node = parse_import("import foo\nfrom \"module\";");

			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "foo");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.named_specifiers().empty());
		}

		SECTION("newline before as in namespace import") {
			auto& node = parse_import("import *\nas ns from \"module\";");

			REQUIRE(node.namespace_name());
			REQUIRE(node.namespace_name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.namespace_name().value() == "ns");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.module_specifier().value() == "module");
			REQUIRE_FALSE(node.default_binding());
			REQUIRE(node.named_specifiers().empty());
		}

		SECTION("newline before with — continues as attributes") {
			auto& node =
				parse_import("import foo from \"m\"\nwith { type: \"json\" };");

			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "foo");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier().value() == "m");
			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(
				node.attributes_keyword()->is<tscc::lex::tokens::with_token>());
			REQUIRE(node.attributes().size() == 1);
			REQUIRE(node.attributes()[0].key().value() == "type");
			REQUIRE(node.attributes()[0].value().value() == "json");
		}

		SECTION("newline before assert — continues as attributes") {
			auto& node = parse_import(
				"import foo from \"m\"\nassert { type: \"json\" };");

			REQUIRE(node.default_binding());
			REQUIRE(node.default_binding()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.default_binding().value() == "foo");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier().value() == "m");
			REQUIRE(node.attributes_keyword() != nullptr);
			REQUIRE(node.attributes_keyword()
						->is<tscc::lex::tokens::assert_token>());
			REQUIRE(node.attributes().size() == 1);
			REQUIRE(node.attributes()[0].key().value() == "type");
			REQUIRE(node.attributes()[0].value().value() == "json");
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
			REQUIRE(first_import->module_specifier());
			REQUIRE(first_import->module_specifier().value() == "a");
			REQUIRE_FALSE(first_import->default_binding());

			++it;
			REQUIRE(it != r.parser->end());
			auto second = std::move(*it);
			REQUIRE(second != nullptr);
			auto* second_import =
				dynamic_cast<tscc::parse::ast::import_node*>(second.get());
			REQUIRE(second_import != nullptr);
			REQUIRE(second_import->module_specifier());
			REQUIRE(second_import->module_specifier().value() == "b");
			REQUIRE_FALSE(second_import->default_binding());
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
			REQUIRE(first_import->default_binding());
			REQUIRE(
				first_import->default_binding()
					->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(first_import->default_binding().value() == "foo");
			REQUIRE(first_import->module_specifier());
			REQUIRE(first_import->module_specifier().value() == "a");

			++it;
			REQUIRE(it != r.parser->end());
			auto second = std::move(*it);
			auto* second_import =
				dynamic_cast<tscc::parse::ast::import_node*>(second.get());
			REQUIRE(second_import != nullptr);
			REQUIRE(second_import->default_binding());
			REQUIRE(
				second_import->default_binding()
					->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(second_import->default_binding().value() == "bar");
			REQUIRE(second_import->module_specifier());
			REQUIRE(second_import->module_specifier().value() == "b");
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

			REQUIRE(node.namespace_name());
			REQUIRE(node.namespace_name()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.namespace_name().value() == "from");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier().value() == "module");
		}

		SECTION("entity identifiers normalized") {
			auto& node = parse_import("import foo = type.from.as;");

			REQUIRE(node.entity_name().size() == 3);
			for (auto& id : node.entity_name().tokens()) {
				REQUIRE(id.is<tscc::lex::tokens::identifier_token>());
			}
			REQUIRE(node.entity_name()[0].value() == "type");
			REQUIRE(node.entity_name()[1].value() == "from");
			REQUIRE(node.entity_name()[2].value() == "as");

			REQUIRE(node.equals_name().value() == "foo");
		}

		SECTION("attribute key normalized") {
			auto& node = parse_import(
				"import foo from \"m\" with { assert: \"json\" };");

			REQUIRE(node.attributes().size() == 1);
			REQUIRE(node.attributes()[0].key());
			REQUIRE(node.attributes()[0]
						.key()
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.attributes()[0].key().value() == "assert");
			REQUIRE(node.attributes()[0].value());
			REQUIRE(node.attributes()[0]
						.value()
						->is<tscc::lex::tokens::constant_value_token>());
			REQUIRE(node.attributes()[0].value().value() == "json");

			REQUIRE(node.default_binding().value() == "foo");
			REQUIRE(node.module_specifier().value() == "m");
		}

		SECTION("specifier alias normalized") {
			auto& node =
				parse_import("import { foo as type } from \"module\";");

			auto& spec = node.named_specifiers()[0];
			REQUIRE(spec.name());
			REQUIRE(spec.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.name().value() == "foo");
			REQUIRE(spec.alias());
			REQUIRE(spec.alias()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.alias().value() == "type");

			REQUIRE(node.module_specifier().value() == "module");
		}

		SECTION("default_token not normalized in specifier name") {
			auto& node =
				parse_import("import { default as foo } from \"module\";");

			auto& spec = node.named_specifiers()[0];
			REQUIRE(spec.is_default());
			REQUIRE(spec.alias());
			REQUIRE(spec.alias()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(spec.alias().value() == "foo");

			REQUIRE(node.module_specifier().value() == "module");
		}
	}
}
