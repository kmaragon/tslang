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
#include <tsclex/tokens/any_token.hpp>
#include <tsclex/tokens/bigint_token.hpp>
#include <tsclex/tokens/boolean_token.hpp>
#include <tsclex/tokens/constant_value_token.hpp>
#include <tsclex/tokens/declare_token.hpp>
#include <tsclex/tokens/false_token.hpp>
#include <tsclex/tokens/identifier_token.hpp>
#include <tsclex/tokens/minus_token.hpp>
#include <tsclex/tokens/never_token.hpp>
#include <tsclex/tokens/null_token.hpp>
#include <tsclex/tokens/number_token.hpp>
#include <tsclex/tokens/object_token.hpp>
#include <tsclex/tokens/string_token.hpp>
#include <tsclex/tokens/symbol_token.hpp>
#include <tsclex/tokens/this_token.hpp>
#include <tsclex/tokens/true_token.hpp>
#include <tsclex/tokens/type_token.hpp>
#include <tsclex/tokens/undefined_token.hpp>
#include <tsclex/tokens/unknown_token.hpp>
#include <tsclex/tokens/void_token.hpp>
#include <tscparse/ast/type/type_alias_node.hpp>
#include <tscparse/ast/type/keyword_type_node.hpp>
#include <tscparse/ast/type/literal_type_node.hpp>
#include <tscparse/ast/type/array_type_node.hpp>
#include <tscparse/ast/type/union_type_node.hpp>
#include <tscparse/ast/type/intersection_type_node.hpp>

#include <tscparse/ast/type/this_type_node.hpp>
#include <tscparse/ast/type/type_reference_node.hpp>
#include <tscparse/ast/namespace_node.hpp>
#include <tscparse/ast/declare_module_node.hpp>
#include <tscparse/error/expected_token.hpp>
#include <tscparse/error/unexpected_end_of_text.hpp>
#include "test_helpers.hpp"

static const tscc::parse::ast::type_alias_node& parse_type_alias(
	const std::string& input) {
	return test_utils::parse_node<tscc::parse::ast::type_alias_node>(input);
}

TEST_CASE("type alias", "[parser][type]") {
	SECTION("basic alias with keyword RHS") {
		SECTION("type Foo = string;") {
			auto& node = parse_type_alias("type Foo = string;");

			REQUIRE(node.declare_keyword() == nullptr);
			REQUIRE(!node.ambient());
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(node.keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.keyword()->location().line() == 0);
			REQUIRE(node.keyword()->location().column() == 0);
			REQUIRE(node.name());
			REQUIRE(node.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.name().value() == "Foo");
			REQUIRE(node.type_parameters().empty());

			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&node.type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::string_token>());
		}
	}

	SECTION("ASI — no semicolon") {
		SECTION("type Foo = string") {
			auto& node = parse_type_alias("type Foo = string");

			REQUIRE(node.declare_keyword() == nullptr);
			REQUIRE(!node.ambient());
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(node.keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.name());
			REQUIRE(node.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.name().value() == "Foo");
			REQUIRE(node.type_parameters().empty());

			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&node.type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::string_token>());
		}
	}

	SECTION("keyword type RHS") {
		SECTION("type T = string;") {
			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&parse_type_alias("type T = string;").type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::string_token>());
		}

		SECTION("type T = number;") {
			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&parse_type_alias("type T = number;").type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::number_token>());
		}

		SECTION("type T = boolean;") {
			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&parse_type_alias("type T = boolean;").type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::boolean_token>());
		}

		SECTION("type T = void;") {
			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&parse_type_alias("type T = void;").type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::void_token>());
		}

		SECTION("type T = never;") {
			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&parse_type_alias("type T = never;").type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::never_token>());
		}

		SECTION("type T = unknown;") {
			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&parse_type_alias("type T = unknown;").type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::unknown_token>());
		}

		SECTION("type T = any;") {
			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&parse_type_alias("type T = any;").type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::any_token>());
		}

		SECTION("type T = object;") {
			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&parse_type_alias("type T = object;").type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::object_token>());
		}

		SECTION("type T = symbol;") {
			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&parse_type_alias("type T = symbol;").type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::symbol_token>());
		}

		SECTION("type T = bigint;") {
			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&parse_type_alias("type T = bigint;").type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::bigint_token>());
		}

		SECTION("type T = undefined;") {
			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&parse_type_alias("type T = undefined;").type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::undefined_token>());
		}

		SECTION("type T = null;") {
			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&parse_type_alias("type T = null;").type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::null_token>());
		}
	}

	SECTION("literal type RHS") {
		SECTION("type T = \"hello\";") {
			auto& node = parse_type_alias(R"(type T = "hello";)");

			REQUIRE(node.declare_keyword() == nullptr);
			REQUIRE(!node.ambient());
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(node.keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.name().value() == "T");
			REQUIRE(node.type_parameters().empty());

			auto* lit =
				dynamic_cast<const tscc::parse::ast::literal_type_node*>(
					&node.type());
			REQUIRE(lit != nullptr);
			REQUIRE(lit->minus_token() == nullptr);
			REQUIRE(lit->value_token()
						.is<tscc::lex::tokens::constant_value_token>());
		}

		SECTION("type T = 42;") {
			auto* lit =
				dynamic_cast<const tscc::parse::ast::literal_type_node*>(
					&parse_type_alias("type T = 42;").type());
			REQUIRE(lit != nullptr);
			REQUIRE(lit->minus_token() == nullptr);
			REQUIRE(lit->value_token()
						.is<tscc::lex::tokens::constant_value_token>());
		}

		SECTION("type T = true;") {
			auto* lit =
				dynamic_cast<const tscc::parse::ast::literal_type_node*>(
					&parse_type_alias("type T = true;").type());
			REQUIRE(lit != nullptr);
			REQUIRE(lit->minus_token() == nullptr);
			REQUIRE(lit->value_token().is<tscc::lex::tokens::true_token>());
		}

		SECTION("type T = false;") {
			auto* lit =
				dynamic_cast<const tscc::parse::ast::literal_type_node*>(
					&parse_type_alias("type T = false;").type());
			REQUIRE(lit != nullptr);
			REQUIRE(lit->minus_token() == nullptr);
			REQUIRE(lit->value_token().is<tscc::lex::tokens::false_token>());
		}

		SECTION("type T = -1;") {
			auto* lit =
				dynamic_cast<const tscc::parse::ast::literal_type_node*>(
					&parse_type_alias("type T = -1;").type());
			REQUIRE(lit != nullptr);
			REQUIRE(lit->minus_token() != nullptr);
			REQUIRE(lit->minus_token()->is<tscc::lex::tokens::minus_token>());
			REQUIRE(lit->value_token()
						.is<tscc::lex::tokens::constant_value_token>());
		}

		SECTION("type T = -0;") {
			auto* lit =
				dynamic_cast<const tscc::parse::ast::literal_type_node*>(
					&parse_type_alias("type T = -0;").type());
			REQUIRE(lit != nullptr);
			REQUIRE(lit->minus_token() != nullptr);
			REQUIRE(lit->minus_token()->is<tscc::lex::tokens::minus_token>());
			REQUIRE(lit->value_token()
						.is<tscc::lex::tokens::constant_value_token>());
		}
	}

	SECTION("this type RHS") {
		SECTION("type T = this;") {
			auto& node = parse_type_alias("type T = this;");

			REQUIRE(node.declare_keyword() == nullptr);
			REQUIRE(!node.ambient());
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(node.keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.name().value() == "T");
			REQUIRE(node.type_parameters().empty());

			auto* th =
				dynamic_cast<const tscc::parse::ast::this_type_node*>(
					&node.type());
			REQUIRE(th != nullptr);
			REQUIRE(th->keyword().is<tscc::lex::tokens::this_token>());
		}
	}

	SECTION("type reference RHS") {
		SECTION("type T = Foo;") {
			auto& node = parse_type_alias("type T = Foo;");

			REQUIRE(node.declare_keyword() == nullptr);
			REQUIRE(!node.ambient());
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(node.keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.name().value() == "T");
			REQUIRE(node.type_parameters().empty());

			auto* ref =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					&node.type());
			REQUIRE(ref != nullptr);
			REQUIRE(ref->name().size() == 1);
			REQUIRE(ref->name()[0]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(ref->name()[0].value() == "Foo");
			REQUIRE(ref->type_arguments().empty());
		}

		SECTION("type Bar = A.B.C;") {
			auto& node = parse_type_alias("type Bar = A.B.C;");

			REQUIRE(node.name().value() == "Bar");
			REQUIRE(node.type_parameters().empty());

			auto* ref =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					&node.type());
			REQUIRE(ref != nullptr);
			REQUIRE(ref->name().size() == 3);
			REQUIRE(ref->name()[0]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(ref->name()[0].value() == "A");
			REQUIRE(ref->name()[1]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(ref->name()[1].value() == "B");
			REQUIRE(ref->name()[2]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(ref->name()[2].value() == "C");
			REQUIRE(ref->type_arguments().empty());
		}

		SECTION("type T = Map<string, number>;") {
			auto& node = parse_type_alias("type T = Map<string, number>;");

			REQUIRE(node.name().value() == "T");
			REQUIRE(node.type_parameters().empty());

			auto* ref =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					&node.type());
			REQUIRE(ref != nullptr);
			REQUIRE(ref->name().size() == 1);
			REQUIRE(ref->name()[0]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(ref->name()[0].value() == "Map");
			REQUIRE(ref->type_arguments().size() == 2);

			auto* arg0 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					ref->type_arguments()[0].get());
			REQUIRE(arg0 != nullptr);
			REQUIRE(arg0->keyword().is<tscc::lex::tokens::string_token>());

			auto* arg1 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					ref->type_arguments()[1].get());
			REQUIRE(arg1 != nullptr);
			REQUIRE(arg1->keyword().is<tscc::lex::tokens::number_token>());
		}

		SECTION("type T = Array<number>;") {
			auto* ref =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					&parse_type_alias("type T = Array<number>;").type());
			REQUIRE(ref != nullptr);
			REQUIRE(ref->name().size() == 1);
			REQUIRE(ref->name()[0]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(ref->name()[0].value() == "Array");
			REQUIRE(ref->type_arguments().size() == 1);

			auto* arg =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					ref->type_arguments()[0].get());
			REQUIRE(arg != nullptr);
			REQUIRE(arg->keyword().is<tscc::lex::tokens::number_token>());
		}
	}

	SECTION("array type RHS") {
		SECTION("type T = string[];") {
			auto& node = parse_type_alias("type T = string[];");

			REQUIRE(node.declare_keyword() == nullptr);
			REQUIRE(!node.ambient());
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(node.keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.name().value() == "T");
			REQUIRE(node.type_parameters().empty());

			auto* arr =
				dynamic_cast<const tscc::parse::ast::array_type_node*>(
					&node.type());
			REQUIRE(arr != nullptr);

			auto* elem =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&arr->element_type());
			REQUIRE(elem != nullptr);
			REQUIRE(elem->keyword().is<tscc::lex::tokens::string_token>());
		}

		SECTION("type T = string[][];") {
			auto* outer =
				dynamic_cast<const tscc::parse::ast::array_type_node*>(
					&parse_type_alias("type T = string[][];").type());
			REQUIRE(outer != nullptr);

			auto* inner =
				dynamic_cast<const tscc::parse::ast::array_type_node*>(
					&outer->element_type());
			REQUIRE(inner != nullptr);

			auto* elem =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&inner->element_type());
			REQUIRE(elem != nullptr);
			REQUIRE(elem->keyword().is<tscc::lex::tokens::string_token>());
		}

		SECTION("type T = Foo[];") {
			auto* arr =
				dynamic_cast<const tscc::parse::ast::array_type_node*>(
					&parse_type_alias("type T = Foo[];").type());
			REQUIRE(arr != nullptr);

			auto* ref =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					&arr->element_type());
			REQUIRE(ref != nullptr);
			REQUIRE(ref->name().size() == 1);
			REQUIRE(ref->name()[0]
						->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(ref->name()[0].value() == "Foo");
			REQUIRE(ref->type_arguments().empty());
		}
	}

	SECTION("union type RHS") {
		SECTION("type Opt = string | number;") {
			auto& node = parse_type_alias("type Opt = string | number;");

			REQUIRE(node.declare_keyword() == nullptr);
			REQUIRE(!node.ambient());
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(node.keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.name().value() == "Opt");
			REQUIRE(node.type_parameters().empty());

			auto* u =
				dynamic_cast<const tscc::parse::ast::union_type_node*>(
					&node.type());
			REQUIRE(u != nullptr);
			REQUIRE(u->members().size() == 2);

			auto* m0 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					u->members()[0].get());
			REQUIRE(m0 != nullptr);
			REQUIRE(m0->keyword().is<tscc::lex::tokens::string_token>());

			auto* m1 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					u->members()[1].get());
			REQUIRE(m1 != nullptr);
			REQUIRE(m1->keyword().is<tscc::lex::tokens::number_token>());
		}

		SECTION("type T = string | number | boolean;") {
			auto* u =
				dynamic_cast<const tscc::parse::ast::union_type_node*>(
					&parse_type_alias(
						"type T = string | number | boolean;").type());
			REQUIRE(u != nullptr);
			REQUIRE(u->members().size() == 3);

			auto* m0 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					u->members()[0].get());
			REQUIRE(m0 != nullptr);
			REQUIRE(m0->keyword().is<tscc::lex::tokens::string_token>());

			auto* m1 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					u->members()[1].get());
			REQUIRE(m1 != nullptr);
			REQUIRE(m1->keyword().is<tscc::lex::tokens::number_token>());

			auto* m2 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					u->members()[2].get());
			REQUIRE(m2 != nullptr);
			REQUIRE(m2->keyword().is<tscc::lex::tokens::boolean_token>());
		}

		SECTION("type T = | string | number; — leading pipe") {
			auto* u =
				dynamic_cast<const tscc::parse::ast::union_type_node*>(
					&parse_type_alias("type T = | string | number;").type());
			REQUIRE(u != nullptr);
			REQUIRE(u->members().size() == 2);

			auto* m0 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					u->members()[0].get());
			REQUIRE(m0 != nullptr);
			REQUIRE(m0->keyword().is<tscc::lex::tokens::string_token>());

			auto* m1 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					u->members()[1].get());
			REQUIRE(m1 != nullptr);
			REQUIRE(m1->keyword().is<tscc::lex::tokens::number_token>());
		}

		SECTION("type T = string | 42 | true | null;") {
			auto* u =
				dynamic_cast<const tscc::parse::ast::union_type_node*>(
					&parse_type_alias(
						"type T = string | 42 | true | null;").type());
			REQUIRE(u != nullptr);
			REQUIRE(u->members().size() == 4);

			auto* m0 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					u->members()[0].get());
			REQUIRE(m0 != nullptr);
			REQUIRE(m0->keyword().is<tscc::lex::tokens::string_token>());

			auto* m1 =
				dynamic_cast<const tscc::parse::ast::literal_type_node*>(
					u->members()[1].get());
			REQUIRE(m1 != nullptr);
			REQUIRE(m1->minus_token() == nullptr);
			REQUIRE(m1->value_token()
						.is<tscc::lex::tokens::constant_value_token>());

			auto* m2 =
				dynamic_cast<const tscc::parse::ast::literal_type_node*>(
					u->members()[2].get());
			REQUIRE(m2 != nullptr);
			REQUIRE(m2->value_token().is<tscc::lex::tokens::true_token>());

			auto* m3 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					u->members()[3].get());
			REQUIRE(m3 != nullptr);
			REQUIRE(m3->keyword().is<tscc::lex::tokens::null_token>());
		}

		SECTION("single type is not a union") {
			auto& t = parse_type_alias("type T = string;").type();
			REQUIRE(
				dynamic_cast<const tscc::parse::ast::union_type_node*>(&t)
				== nullptr);
			REQUIRE(
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(&t)
				!= nullptr);
		}
	}

	SECTION("intersection type RHS") {
		SECTION("type Both = A & B;") {
			auto& node = parse_type_alias("type Both = A & B;");

			REQUIRE(node.declare_keyword() == nullptr);
			REQUIRE(!node.ambient());
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(node.keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.name().value() == "Both");
			REQUIRE(node.type_parameters().empty());

			auto* i =
				dynamic_cast<const tscc::parse::ast::intersection_type_node*>(
					&node.type());
			REQUIRE(i != nullptr);
			REQUIRE(i->members().size() == 2);

			auto* m0 =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					i->members()[0].get());
			REQUIRE(m0 != nullptr);
			REQUIRE(m0->name().size() == 1);
			REQUIRE(m0->name()[0].value() == "A");
			REQUIRE(m0->type_arguments().empty());

			auto* m1 =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					i->members()[1].get());
			REQUIRE(m1 != nullptr);
			REQUIRE(m1->name().size() == 1);
			REQUIRE(m1->name()[0].value() == "B");
			REQUIRE(m1->type_arguments().empty());
		}

		SECTION("type T = A & B & C;") {
			auto* i =
				dynamic_cast<const tscc::parse::ast::intersection_type_node*>(
					&parse_type_alias("type T = A & B & C;").type());
			REQUIRE(i != nullptr);
			REQUIRE(i->members().size() == 3);

			auto* m0 =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					i->members()[0].get());
			REQUIRE(m0 != nullptr);
			REQUIRE(m0->name()[0].value() == "A");

			auto* m1 =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					i->members()[1].get());
			REQUIRE(m1 != nullptr);
			REQUIRE(m1->name()[0].value() == "B");

			auto* m2 =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					i->members()[2].get());
			REQUIRE(m2 != nullptr);
			REQUIRE(m2->name()[0].value() == "C");
		}

		SECTION("type T = & A & B; — leading ampersand") {
			auto* i =
				dynamic_cast<const tscc::parse::ast::intersection_type_node*>(
					&parse_type_alias("type T = & A & B;").type());
			REQUIRE(i != nullptr);
			REQUIRE(i->members().size() == 2);

			auto* m0 =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					i->members()[0].get());
			REQUIRE(m0 != nullptr);
			REQUIRE(m0->name()[0].value() == "A");

			auto* m1 =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					i->members()[1].get());
			REQUIRE(m1 != nullptr);
			REQUIRE(m1->name()[0].value() == "B");
		}

		SECTION("single type is not an intersection") {
			auto& t = parse_type_alias("type T = Foo;").type();
			REQUIRE(
				dynamic_cast<
					const tscc::parse::ast::intersection_type_node*>(&t)
				== nullptr);
		}
	}

	SECTION("precedence") {
		SECTION("type T = A | B & C; — union vs intersection") {
			auto* u =
				dynamic_cast<const tscc::parse::ast::union_type_node*>(
					&parse_type_alias("type T = A | B & C;").type());
			REQUIRE(u != nullptr);
			REQUIRE(u->members().size() == 2);

			auto* first =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					u->members()[0].get());
			REQUIRE(first != nullptr);
			REQUIRE(first->name()[0].value() == "A");

			auto* second =
				dynamic_cast<const tscc::parse::ast::intersection_type_node*>(
					u->members()[1].get());
			REQUIRE(second != nullptr);
			REQUIRE(second->members().size() == 2);

			auto* b =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					second->members()[0].get());
			REQUIRE(b != nullptr);
			REQUIRE(b->name()[0].value() == "B");

			auto* c =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					second->members()[1].get());
			REQUIRE(c != nullptr);
			REQUIRE(c->name()[0].value() == "C");
		}

		SECTION("type T = string[] | number; — array higher than union") {
			auto* u =
				dynamic_cast<const tscc::parse::ast::union_type_node*>(
					&parse_type_alias("type T = string[] | number;").type());
			REQUIRE(u != nullptr);
			REQUIRE(u->members().size() == 2);

			auto* arr =
				dynamic_cast<const tscc::parse::ast::array_type_node*>(
					u->members()[0].get());
			REQUIRE(arr != nullptr);

			auto* elem =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&arr->element_type());
			REQUIRE(elem != nullptr);
			REQUIRE(elem->keyword().is<tscc::lex::tokens::string_token>());

			auto* num =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					u->members()[1].get());
			REQUIRE(num != nullptr);
			REQUIRE(num->keyword().is<tscc::lex::tokens::number_token>());
		}

		SECTION("type T = A[] & B; — array higher than intersection") {
			auto* i =
				dynamic_cast<const tscc::parse::ast::intersection_type_node*>(
					&parse_type_alias("type T = A[] & B;").type());
			REQUIRE(i != nullptr);
			REQUIRE(i->members().size() == 2);

			auto* arr =
				dynamic_cast<const tscc::parse::ast::array_type_node*>(
					i->members()[0].get());
			REQUIRE(arr != nullptr);

			auto* elem =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					&arr->element_type());
			REQUIRE(elem != nullptr);
			REQUIRE(elem->name()[0].value() == "A");

			auto* b =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					i->members()[1].get());
			REQUIRE(b != nullptr);
			REQUIRE(b->name()[0].value() == "B");
		}
	}

	SECTION("parenthesized type RHS") {
		SECTION("type T = (string); — parens pass through inner type") {
			auto& node = parse_type_alias("type T = (string);");

			REQUIRE(node.name().value() == "T");
			REQUIRE(node.type_parameters().empty());

			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&node.type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::string_token>());
		}

		SECTION("type T = (A | B)[]; — parenthesized union then array") {
			auto* arr =
				dynamic_cast<const tscc::parse::ast::array_type_node*>(
					&parse_type_alias("type T = (A | B)[];").type());
			REQUIRE(arr != nullptr);

			auto* u =
				dynamic_cast<const tscc::parse::ast::union_type_node*>(
					&arr->element_type());
			REQUIRE(u != nullptr);
			REQUIRE(u->members().size() == 2);

			auto* a =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					u->members()[0].get());
			REQUIRE(a != nullptr);
			REQUIRE(a->name()[0].value() == "A");

			auto* b =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					u->members()[1].get());
			REQUIRE(b != nullptr);
			REQUIRE(b->name()[0].value() == "B");
		}

		SECTION("type T = ((string)); — nested parens pass through") {
			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&parse_type_alias("type T = ((string));").type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::string_token>());
		}

		SECTION("type T = (A & B)[]; — parenthesized intersection then array") {
			auto* arr =
				dynamic_cast<const tscc::parse::ast::array_type_node*>(
					&parse_type_alias("type T = (A & B)[];").type());
			REQUIRE(arr != nullptr);

			auto* i =
				dynamic_cast<const tscc::parse::ast::intersection_type_node*>(
					&arr->element_type());
			REQUIRE(i != nullptr);
			REQUIRE(i->members().size() == 2);
		}
	}

	SECTION("complex combinations") {
		SECTION("type T = string[] | number[];") {
			auto* u =
				dynamic_cast<const tscc::parse::ast::union_type_node*>(
					&parse_type_alias("type T = string[] | number[];").type());
			REQUIRE(u != nullptr);
			REQUIRE(u->members().size() == 2);

			auto* arr0 =
				dynamic_cast<const tscc::parse::ast::array_type_node*>(
					u->members()[0].get());
			REQUIRE(arr0 != nullptr);
			auto* elem0 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&arr0->element_type());
			REQUIRE(elem0 != nullptr);
			REQUIRE(elem0->keyword().is<tscc::lex::tokens::string_token>());

			auto* arr1 =
				dynamic_cast<const tscc::parse::ast::array_type_node*>(
					u->members()[1].get());
			REQUIRE(arr1 != nullptr);
			auto* elem1 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&arr1->element_type());
			REQUIRE(elem1 != nullptr);
			REQUIRE(elem1->keyword().is<tscc::lex::tokens::number_token>());
		}

		SECTION("type T = Foo<string> & Bar<number>;") {
			auto* i =
				dynamic_cast<const tscc::parse::ast::intersection_type_node*>(
					&parse_type_alias(
						"type T = Foo<string> & Bar<number>;").type());
			REQUIRE(i != nullptr);
			REQUIRE(i->members().size() == 2);

			auto* ref0 =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					i->members()[0].get());
			REQUIRE(ref0 != nullptr);
			REQUIRE(ref0->name().size() == 1);
			REQUIRE(ref0->name()[0].value() == "Foo");
			REQUIRE(ref0->type_arguments().size() == 1);
			auto* arg0 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					ref0->type_arguments()[0].get());
			REQUIRE(arg0 != nullptr);
			REQUIRE(arg0->keyword().is<tscc::lex::tokens::string_token>());

			auto* ref1 =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					i->members()[1].get());
			REQUIRE(ref1 != nullptr);
			REQUIRE(ref1->name().size() == 1);
			REQUIRE(ref1->name()[0].value() == "Bar");
			REQUIRE(ref1->type_arguments().size() == 1);
			auto* arg1 =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					ref1->type_arguments()[0].get());
			REQUIRE(arg1 != nullptr);
			REQUIRE(arg1->keyword().is<tscc::lex::tokens::number_token>());
		}
	}

	SECTION("type parameters") {
		SECTION("type Generic<T> = T[];") {
			auto& node = parse_type_alias("type Generic<T> = T[];");

			REQUIRE(node.declare_keyword() == nullptr);
			REQUIRE(!node.ambient());
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(node.keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.name().value() == "Generic");
			REQUIRE(node.type_parameters().size() == 1);

			auto& param = *node.type_parameters()[0];
			REQUIRE(param.name());
			REQUIRE(param.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(param.name().value() == "T");
			REQUIRE(!param.has_in());
			REQUIRE(!param.has_out());
			REQUIRE(param.constraint() == nullptr);
			REQUIRE(param.default_type() == nullptr);

			auto* arr =
				dynamic_cast<const tscc::parse::ast::array_type_node*>(
					&node.type());
			REQUIRE(arr != nullptr);

			auto* elem =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					&arr->element_type());
			REQUIRE(elem != nullptr);
			REQUIRE(elem->name().size() == 1);
			REQUIRE(elem->name()[0].value() == "T");
			REQUIRE(elem->type_arguments().empty());
		}

		SECTION("type Map<K, V> = Record<K, V>;") {
			auto& node = parse_type_alias("type Map<K, V> = Record<K, V>;");

			REQUIRE(node.name().value() == "Map");
			REQUIRE(node.type_parameters().size() == 2);

			auto& k = *node.type_parameters()[0];
			REQUIRE(k.name());
			REQUIRE(k.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(k.name().value() == "K");
			REQUIRE(!k.has_in());
			REQUIRE(!k.has_out());
			REQUIRE(k.constraint() == nullptr);
			REQUIRE(k.default_type() == nullptr);

			auto& v = *node.type_parameters()[1];
			REQUIRE(v.name());
			REQUIRE(v.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(v.name().value() == "V");
			REQUIRE(!v.has_in());
			REQUIRE(!v.has_out());
			REQUIRE(v.constraint() == nullptr);
			REQUIRE(v.default_type() == nullptr);

			auto* ref =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					&node.type());
			REQUIRE(ref != nullptr);
			REQUIRE(ref->name().size() == 1);
			REQUIRE(ref->name()[0].value() == "Record");
			REQUIRE(ref->type_arguments().size() == 2);

			auto* targ0 =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					ref->type_arguments()[0].get());
			REQUIRE(targ0 != nullptr);
			REQUIRE(targ0->name()[0].value() == "K");
			REQUIRE(targ0->type_arguments().empty());

			auto* targ1 =
				dynamic_cast<const tscc::parse::ast::type_reference_node*>(
					ref->type_arguments()[1].get());
			REQUIRE(targ1 != nullptr);
			REQUIRE(targ1->name()[0].value() == "V");
			REQUIRE(targ1->type_arguments().empty());
		}

		SECTION("type Bounded<T extends string> = T;") {
			auto& node =
				parse_type_alias("type Bounded<T extends string> = T;");

			REQUIRE(node.name().value() == "Bounded");
			REQUIRE(node.type_parameters().size() == 1);

			auto& param = *node.type_parameters()[0];
			REQUIRE(param.name().value() == "T");
			REQUIRE(!param.has_in());
			REQUIRE(!param.has_out());
			REQUIRE(param.constraint() != nullptr);
			REQUIRE(param.default_type() == nullptr);

			auto* constraint =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					param.constraint());
			REQUIRE(constraint != nullptr);
			REQUIRE(
				constraint->keyword().is<tscc::lex::tokens::string_token>());
		}

		SECTION("type WithDefault<T = string> = T;") {
			auto& node =
				parse_type_alias("type WithDefault<T = string> = T;");

			REQUIRE(node.name().value() == "WithDefault");
			REQUIRE(node.type_parameters().size() == 1);

			auto& param = *node.type_parameters()[0];
			REQUIRE(param.name().value() == "T");
			REQUIRE(!param.has_in());
			REQUIRE(!param.has_out());
			REQUIRE(param.constraint() == nullptr);
			REQUIRE(param.default_type() != nullptr);

			auto* def =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					param.default_type());
			REQUIRE(def != nullptr);
			REQUIRE(def->keyword().is<tscc::lex::tokens::string_token>());
		}

		SECTION(R"(type Full<T extends string = "hi"> = T;)") {
			auto& node = parse_type_alias(
				R"(type Full<T extends string = "hi"> = T;)");

			REQUIRE(node.name().value() == "Full");
			REQUIRE(node.type_parameters().size() == 1);

			auto& param = *node.type_parameters()[0];
			REQUIRE(param.name().value() == "T");
			REQUIRE(!param.has_in());
			REQUIRE(!param.has_out());
			REQUIRE(param.constraint() != nullptr);
			REQUIRE(param.default_type() != nullptr);

			auto* constraint =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					param.constraint());
			REQUIRE(constraint != nullptr);
			REQUIRE(
				constraint->keyword().is<tscc::lex::tokens::string_token>());

			auto* def =
				dynamic_cast<const tscc::parse::ast::literal_type_node*>(
					param.default_type());
			REQUIRE(def != nullptr);
			REQUIRE(def->minus_token() == nullptr);
			REQUIRE(def->value_token()
						.is<tscc::lex::tokens::constant_value_token>());
		}

		SECTION("variance modifiers") {
			SECTION("type Contra<in T> = T;") {
				auto& node = parse_type_alias("type Contra<in T> = T;");

				REQUIRE(node.type_parameters().size() == 1);

				auto& param = *node.type_parameters()[0];
				REQUIRE(param.name().value() == "T");
				REQUIRE(param.has_in());
				REQUIRE(!param.has_out());
				REQUIRE(param.constraint() == nullptr);
				REQUIRE(param.default_type() == nullptr);
			}

			SECTION("type Covar<out T> = T;") {
				auto& node = parse_type_alias("type Covar<out T> = T;");

				REQUIRE(node.type_parameters().size() == 1);

				auto& param = *node.type_parameters()[0];
				REQUIRE(param.name().value() == "T");
				REQUIRE(!param.has_in());
				REQUIRE(param.has_out());
				REQUIRE(param.constraint() == nullptr);
				REQUIRE(param.default_type() == nullptr);
			}

			SECTION("type Invariant<in out T> = T;") {
				auto& node =
					parse_type_alias("type Invariant<in out T> = T;");

				REQUIRE(node.type_parameters().size() == 1);

				auto& param = *node.type_parameters()[0];
				REQUIRE(param.name().value() == "T");
				REQUIRE(param.has_in());
				REQUIRE(param.has_out());
				REQUIRE(param.constraint() == nullptr);
				REQUIRE(param.default_type() == nullptr);
			}
		}
	}

	SECTION("declare type alias") {
		SECTION("declare type Foo = string;") {
			auto& node = parse_type_alias("declare type Foo = string;");

			REQUIRE(node.declare_keyword() != nullptr);
			REQUIRE(
				node.declare_keyword()->is<tscc::lex::tokens::declare_token>());
			REQUIRE(node.declare_keyword()->location().line() == 0);
			REQUIRE(node.declare_keyword()->location().column() == 0);
			REQUIRE(node.ambient());
			REQUIRE(node.keyword() != nullptr);
			REQUIRE(node.keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(node.keyword()->location().line() == 0);
			REQUIRE(node.keyword()->location().column() == 8);
			REQUIRE(node.name());
			REQUIRE(node.name()->is<tscc::lex::tokens::identifier_token>());
			REQUIRE(node.name().value() == "Foo");
			REQUIRE(node.type_parameters().empty());

			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&node.type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::string_token>());
		}
	}

	SECTION("type alias in namespace") {
		SECTION("namespace N { type Foo = string; }") {
			auto result = test_utils::parse_source(
				"namespace N { type Foo = string; }");
			auto& ns =
				dynamic_cast<const tscc::parse::ast::namespace_node&>(
					*result.root->children().front());
			REQUIRE(ns.children().size() == 1);

			auto* alias =
				dynamic_cast<const tscc::parse::ast::type_alias_node*>(
					ns.children()[0].get());
			REQUIRE(alias != nullptr);
			REQUIRE(alias->parent() == &ns);
			REQUIRE(alias->declare_keyword() == nullptr);
			REQUIRE(!alias->ambient());
			REQUIRE(alias->keyword() != nullptr);
			REQUIRE(alias->keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(alias->name().value() == "Foo");
			REQUIRE(alias->type_parameters().empty());

			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&alias->type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::string_token>());
		}
	}

	SECTION("type alias in declare module") {
		SECTION(R"(declare module "m" { type Foo = string; })") {
			auto result = test_utils::parse_source(
				R"(declare module "m" { type Foo = string; })");
			auto& dm =
				dynamic_cast<const tscc::parse::ast::declare_module_node&>(
					*result.root->children().front());
			REQUIRE(dm.children().size() == 1);

			auto* alias =
				dynamic_cast<const tscc::parse::ast::type_alias_node*>(
					dm.children()[0].get());
			REQUIRE(alias != nullptr);
			REQUIRE(alias->parent() == &dm);
			REQUIRE(alias->declare_keyword() == nullptr);
			REQUIRE(!alias->ambient());
			REQUIRE(alias->keyword() != nullptr);
			REQUIRE(alias->keyword()->is<tscc::lex::tokens::type_token>());
			REQUIRE(alias->name().value() == "Foo");
			REQUIRE(alias->type_parameters().empty());

			auto* kw =
				dynamic_cast<const tscc::parse::ast::keyword_type_node*>(
					&alias->type());
			REQUIRE(kw != nullptr);
			REQUIRE(kw->keyword().is<tscc::lex::tokens::string_token>());
		}
	}

	SECTION("source locations") {
		SECTION("type keyword location") {
			auto& node = parse_type_alias("type Foo = string;");

			REQUIRE(node.keyword()->location().line() == 0);
			REQUIRE(node.keyword()->location().column() == 0);
		}

		SECTION("type alias on second line") {
			auto& node = parse_type_alias("\ntype Foo = string;");

			REQUIRE(node.keyword()->location().line() == 1);
			REQUIRE(node.keyword()->location().column() == 0);
		}

		SECTION("name location") {
			auto& node = parse_type_alias("type Foo = string;");

			REQUIRE(node.name()->location().line() == 0);
			REQUIRE(node.name()->location().column() == 5);
		}
	}

	SECTION("parse errors") {
		SECTION("missing name") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type = string;"),
				tscc::parse::expected_token);
		}

		SECTION("missing equals") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type Foo string;"),
				tscc::parse::expected_token);
		}

		SECTION("missing RHS type") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type Foo = ;"),
				tscc::parse::expected_token);
		}

		SECTION("empty type parameter list") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type Foo<> = string;"),
				tscc::parse::expected_token);
		}

		SECTION("duplicate in modifier") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type Foo<in in T> = T;"),
				tscc::parse::expected_token);
		}

		SECTION("duplicate out modifier") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type Foo<out out T> = T;"),
				tscc::parse::expected_token);
		}

		SECTION("wrong order out in") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type Foo<out in T> = T;"),
				tscc::parse::expected_token);
		}

		SECTION("missing type after |") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type T = A |;"),
				tscc::parse::expected_token);
		}

		SECTION("missing type after &") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type T = A &;"),
				tscc::parse::expected_token);
		}

		SECTION("unclosed parenthesized type") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type T = (string;"),
				tscc::parse::expected_token);
		}

		SECTION("trailing dot in qualified name") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type T = A.;"),
				tscc::parse::expected_token);
		}

		SECTION("unclosed type argument list") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type T = Map<string;"),
				tscc::parse::expected_token);
		}

		SECTION("minus without numeric literal") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type T = -string;"),
				tscc::parse::expected_token);
		}

		SECTION("empty parentheses") {
			REQUIRE_THROWS_AS(
				test_utils::parse_source("type T = ();"),
				tscc::parse::expected_token);
		}
	}
}
