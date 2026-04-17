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
#include <tsclex/tokens/asterisk_token.hpp>
#include <tsclex/tokens/declare_token.hpp>
#include <tsclex/tokens/eq_token.hpp>
#include <tsclex/tokens/export_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/module_token.hpp>
#include <tsclex/tokens/namespace_token.hpp>
#include <tsclex/tokens/type_token.hpp>
#include <tscparse/ast/export_assignment_node.hpp>
#include <tscparse/ast/export_star_node.hpp>
#include <tscparse/ast/import_node.hpp>
#include <tscparse/ast/named_export_node.hpp>
#include <tscparse/ast/namespace_node.hpp>
#include <tscparse/ast/type_node.hpp>
#include <tscparse/error/declaration_or_statement_expected.hpp>
#include <tscparse/error/expected_token.hpp>
#include <tscparse/error/export_assignment_conflicts.hpp>
#include <tscparse/error/export_assignment_not_at_top_level.hpp>
#include <tscparse/error/export_default_not_at_top_level.hpp>
#include <tscparse/error/unexpected_end_of_text.hpp>
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

	SECTION("export default / export = validation") {
		SECTION("export default in namespace throws TS1258") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source(
					"namespace Foo { export default }"),
				tscc::parse::export_default_not_at_top_level);
		}

		SECTION("export = in namespace throws TS1231") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source(
					"namespace Foo { export = }"),
				tscc::parse::export_assignment_not_at_top_level);
		}

		SECTION("bare export default at module scope throws TS1128") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("export default"),
				tscc::parse::declaration_or_statement_expected);
		}

		SECTION("export default with valid statement") {
			// TODO: should parse successfully once export_default_state exists
			REQUIRE_THROWS_AS(
				test_utils::parse_source("export default class Foo {}"),
				tscc::parse::declaration_or_statement_expected);
		}

		SECTION("bare export = at module scope throws unexpected EOF") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("export ="),
				tscc::parse::unexpected_end_of_text);
		}

		SECTION("export = non-identifier throws expected token") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("export = ;"),
				tscc::parse::expected_token);
		}
	}

	SECTION("export assignment") {
		SECTION("export = Foo;") {
			auto& node =
				test_utils::parse_node<tscc::parse::ast::export_assignment_node>(
					"export = Foo;");

			REQUIRE(node.export_keyword() != nullptr);
			REQUIRE(
				node.export_keyword()->is<tscc::lex::tokens::export_token>());
			REQUIRE(node.equals_token() != nullptr);
			REQUIRE(node.equals_token()->is<tscc::lex::tokens::eq_token>());
			REQUIRE(node.identifier());
			REQUIRE(node.identifier().value() == "Foo");
		}

		SECTION("ASI: export = Foo at EOF") {
			auto& node =
				test_utils::parse_node<tscc::parse::ast::export_assignment_node>(
					"export = Foo");

			REQUIRE(node.export_keyword() != nullptr);
			REQUIRE(node.identifier());
			REQUIRE(node.identifier().value() == "Foo");
		}

		SECTION("ASI: export = Foo followed by next statement") {
			auto r = test_utils::parse_source(
				"export = Foo\ntype Bar = string");
			REQUIRE(r.root != nullptr);
			REQUIRE(r.root->children().size() == 2);

			auto* ea = dynamic_cast<
				const tscc::parse::ast::export_assignment_node*>(
				r.root->children()[0].get());
			REQUIRE(ea != nullptr);
			REQUIRE(ea->identifier().value() == "Foo");

			auto* ty = dynamic_cast<const tscc::parse::ast::type_node*>(
				r.root->children()[1].get());
			REQUIRE(ty != nullptr);
			REQUIRE(ty->name().value() == "Bar");
		}

		SECTION("export = Foo; export type Bar = string; throws TS2309") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source(
					"export = Foo; export type Bar = string;"),
				tscc::parse::export_assignment_conflicts);
		}

		SECTION("export type Bar = string; export = Foo; throws TS2309") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source(
					"export type Bar = string; export = Foo;"),
				tscc::parse::export_assignment_conflicts);
		}

		SECTION("export = Foo; export = Bar; throws TS2309") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("export = Foo; export = Bar;"),
				tscc::parse::export_assignment_conflicts);
		}
	}

	SECTION("export star") {
		SECTION("export * from \"module\";") {
			auto& node =
				test_utils::parse_node<tscc::parse::ast::export_star_node>(
					"export * from \"module\";");

			REQUIRE(node.export_keyword() != nullptr);
			REQUIRE(
				node.export_keyword()->is<tscc::lex::tokens::export_token>());
			REQUIRE(node.asterisk() != nullptr);
			REQUIRE(
				node.asterisk()->is<tscc::lex::tokens::asterisk_token>());
			REQUIRE_FALSE(node.namespace_name());
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier().value() == "module");
		}

		SECTION("export * as ns from \"module\";") {
			auto& node =
				test_utils::parse_node<tscc::parse::ast::export_star_node>(
					"export * as ns from \"module\";");

			REQUIRE(node.export_keyword() != nullptr);
			REQUIRE(node.namespace_name());
			REQUIRE(node.namespace_name().value() == "ns");
			REQUIRE(node.module_specifier());
			REQUIRE(node.module_specifier().value() == "module");
		}

		SECTION("ASI: export * from \"module\" at EOF") {
			auto& node =
				test_utils::parse_node<tscc::parse::ast::export_star_node>(
					"export * from \"module\"");

			REQUIRE(node.export_keyword() != nullptr);
			REQUIRE(node.module_specifier().value() == "module");
		}

		SECTION("ASI: export * followed by next statement") {
			auto r = test_utils::parse_source(
				"export * from \"a\"\nexport type Foo = string");
			REQUIRE(r.root != nullptr);
			REQUIRE(r.root->children().size() == 2);

			auto* star = dynamic_cast<
				const tscc::parse::ast::export_star_node*>(
				r.root->children()[0].get());
			REQUIRE(star != nullptr);
			REQUIRE(star->module_specifier().value() == "a");

			auto* ty = dynamic_cast<const tscc::parse::ast::type_node*>(
				r.root->children()[1].get());
			REQUIRE(ty != nullptr);
			REQUIRE(ty->name().value() == "Foo");
		}

		SECTION("ASI: export * as ns followed by next statement") {
			auto r = test_utils::parse_source(
				"export * as ns from \"a\"\nexport type Foo = string");
			REQUIRE(r.root != nullptr);
			REQUIRE(r.root->children().size() == 2);

			auto* star = dynamic_cast<
				const tscc::parse::ast::export_star_node*>(
				r.root->children()[0].get());
			REQUIRE(star != nullptr);
			REQUIRE(star->namespace_name().value() == "ns");
			REQUIRE(star->module_specifier().value() == "a");
		}

		SECTION("multiple star exports coexist") {
			auto r = test_utils::parse_source(
				"export * from \"a\"; export * from \"b\";");
			REQUIRE(r.root != nullptr);
			REQUIRE(r.root->children().size() == 2);
		}

		SECTION("export = then export * throws TS2309") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source(
					"export = Foo; export * from \"module\";"),
				tscc::parse::export_assignment_conflicts);
		}

		SECTION("export * then export = throws TS2309") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source(
					"export * from \"module\"; export = Foo;"),
				tscc::parse::export_assignment_conflicts);
		}

		SECTION("missing from throws expected token") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("export * \"module\";"),
				tscc::parse::expected_token);
		}

		SECTION("missing identifier after as throws expected token") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("export * as ;"),
				tscc::parse::expected_token);
		}

		SECTION("missing module specifier throws expected token") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("export * from ;"),
				tscc::parse::expected_token);
		}

		SECTION("export * at EOF throws unexpected end of text") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("export *"),
				tscc::parse::unexpected_end_of_text);
		}
	}

	SECTION("named exports") {
		SECTION("basic parsing") {
			SECTION("export { };") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { };");

				REQUIRE(node.export_keyword() != nullptr);
				REQUIRE(
					node.export_keyword()->is<tscc::lex::tokens::export_token>());
				REQUIRE(node.specifiers().empty());
				REQUIRE_FALSE(node.module_specifier());
			}

			SECTION("export { a };") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { a };");

				REQUIRE(node.specifiers().size() == 1);
				REQUIRE(node.specifiers()[0].name().value() == "a");
				REQUIRE_FALSE(node.specifiers()[0].is_default());
				REQUIRE(node.specifiers()[0].type_keyword() == nullptr);
				REQUIRE_FALSE(node.specifiers()[0].alias());
			}

			SECTION("export { a, b, c };") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { a, b, c };");

				REQUIRE(node.specifiers().size() == 3);
				REQUIRE(node.specifiers()[0].name().value() == "a");
				REQUIRE(node.specifiers()[1].name().value() == "b");
				REQUIRE(node.specifiers()[2].name().value() == "c");
			}

			SECTION("export { a as b };") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { a as b };");

				REQUIRE(node.specifiers().size() == 1);
				REQUIRE(node.specifiers()[0].name().value() == "a");
				REQUIRE(node.specifiers()[0].alias());
				REQUIRE(node.specifiers()[0].alias().value() == "b");
			}

			SECTION("export { a, b as c, d };") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { a, b as c, d };");

				REQUIRE(node.specifiers().size() == 3);
				REQUIRE(node.specifiers()[0].name().value() == "a");
				REQUIRE_FALSE(node.specifiers()[0].alias());
				REQUIRE(node.specifiers()[1].name().value() == "b");
				REQUIRE(node.specifiers()[1].alias().value() == "c");
				REQUIRE(node.specifiers()[2].name().value() == "d");
				REQUIRE_FALSE(node.specifiers()[2].alias());
			}
		}

		SECTION("type modifier") {
			SECTION("export { type a };") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { type a };");

				REQUIRE(node.specifiers().size() == 1);
				REQUIRE(node.specifiers()[0].type_keyword() != nullptr);
				REQUIRE(node.specifiers()[0].name().value() == "a");
			}

			SECTION("export { type a as b };") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { type a as b };");

				REQUIRE(node.specifiers().size() == 1);
				REQUIRE(node.specifiers()[0].type_keyword() != nullptr);
				REQUIRE(node.specifiers()[0].name().value() == "a");
				REQUIRE(node.specifiers()[0].alias().value() == "b");
			}

			SECTION("export { type };") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { type };");

				REQUIRE(node.specifiers().size() == 1);
				REQUIRE(node.specifiers()[0].type_keyword() == nullptr);
				REQUIRE(node.specifiers()[0].name().value() == "type");
			}
		}

		SECTION("contextual keywords as names") {
			SECTION("export { default as name };") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { default as name };");

				REQUIRE(node.specifiers().size() == 1);
				REQUIRE(node.specifiers()[0].is_default());
				REQUIRE(node.specifiers()[0].alias().value() == "name");
			}

			SECTION("export { from };") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { from };");

				REQUIRE(node.specifiers().size() == 1);
				REQUIRE(node.specifiers()[0].name().value() == "from");
			}

			SECTION("export { as };") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { as };");

				REQUIRE(node.specifiers().size() == 1);
				REQUIRE(node.specifiers()[0].name().value() == "as");
			}
		}

		SECTION("trailing comma") {
			SECTION("export { a, };") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { a, };");

				REQUIRE(node.specifiers().size() == 1);
				REQUIRE(node.specifiers()[0].name().value() == "a");
			}
		}

		SECTION("re-export with from") {
			SECTION("export { a } from \"module\";") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { a } from \"module\";");

				REQUIRE(node.specifiers().size() == 1);
				REQUIRE(node.specifiers()[0].name().value() == "a");
				REQUIRE(node.module_specifier());
				REQUIRE(node.module_specifier().value() == "module");
			}

			SECTION("export { a, b as c } from \"module\";") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { a, b as c } from \"module\";");

				REQUIRE(node.specifiers().size() == 2);
				REQUIRE(node.specifiers()[0].name().value() == "a");
				REQUIRE(node.specifiers()[1].name().value() == "b");
				REQUIRE(node.specifiers()[1].alias().value() == "c");
				REQUIRE(node.module_specifier().value() == "module");
			}
		}

		SECTION("ASI") {
			SECTION("export { a } at EOF") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { a }");

				REQUIRE(node.specifiers().size() == 1);
				REQUIRE(node.specifiers()[0].name().value() == "a");
			}

			SECTION("export { a } from \"module\" at EOF") {
				auto& node =
					test_utils::parse_node<tscc::parse::ast::named_export_node>(
						"export { a } from \"module\"");

				REQUIRE(node.specifiers().size() == 1);
				REQUIRE(node.module_specifier().value() == "module");
			}

			SECTION("export { a } followed by next statement") {
				auto r = test_utils::parse_source(
					"export { a }\nexport type Foo = string");
				REQUIRE(r.root != nullptr);
				REQUIRE(r.root->children().size() == 2);

				auto* ne = dynamic_cast<
					const tscc::parse::ast::named_export_node*>(
					r.root->children()[0].get());
				REQUIRE(ne != nullptr);
				REQUIRE(ne->specifiers().size() == 1);
				REQUIRE(ne->specifiers()[0].name().value() == "a");

				auto* ty = dynamic_cast<const tscc::parse::ast::type_node*>(
					r.root->children()[1].get());
				REQUIRE(ty != nullptr);
				REQUIRE(ty->name().value() == "Foo");
			}
		}

		SECTION("TS2309 conflicts") {
			SECTION("export = then export { } throws TS2309") {
				REQUIRE_THROWS_AS(
					test_utils::parse_source(
						"export = Foo; export { a };"),
					tscc::parse::export_assignment_conflicts);
			}

			SECTION("export { } then export = throws TS2309") {
				REQUIRE_THROWS_AS(
					test_utils::parse_source(
						"export { a }; export = Foo;"),
					tscc::parse::export_assignment_conflicts);
			}
		}

		SECTION("error cases") {
			SECTION("export { a b } — missing comma") {
				REQUIRE_THROWS_AS(
					test_utils::parse_source("export { a b };"),
					tscc::parse::expected_token);
			}

			SECTION("export { , } — leading comma") {
				REQUIRE_THROWS_AS(
					test_utils::parse_source("export { , };"),
					tscc::parse::expected_token);
			}

			SECTION("export { at EOF") {
				REQUIRE_THROWS_AS(
					test_utils::parse_source("export {"),
					tscc::parse::unexpected_end_of_text);
			}

			SECTION("export { a } from ; — missing module specifier") {
				REQUIRE_THROWS_AS(
					test_utils::parse_source("export { a } from ;"),
					tscc::parse::expected_token);
			}
		}
	}

	SECTION("multiple named exports coexist") {
		auto r = test_utils::parse_source(
			"export type Foo = string; export type Bar = number;");
		REQUIRE(r.root != nullptr);
		REQUIRE(r.root->children().size() == 2);
	}
}
