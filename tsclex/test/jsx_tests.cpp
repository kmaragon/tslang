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

TEST_CASE("JSX Tokens", "[lexer]") {
	auto [file, source, create_lexer, tokenize] = test_utils::create_test_setup();
	source->language_variant(tscc::lex::ts_language_variant::jsx);

	SECTION("Simple JSX Element") {
		// <div>Hello</div>
		auto tokens = tokenize("var div = <div>Hello</div>");
		REQUIRE(tokens.size() == 7);  // <div, >, Hello, </div>

		// var
		CHECK(tokens[0].is<tscc::lex::tokens::var_token>());

		// div
		CHECK(tokens[1].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[1]->to_string() == "div");

		// =
		CHECK(tokens[2].is<tscc::lex::tokens::eq_token>());

		// <div
		CHECK(tokens[3].is<tscc::lex::tokens::jsx_element_start_token>());
		CHECK(tokens[3]->to_string() == "<div");

		// >
		CHECK(tokens[4].is<tscc::lex::tokens::jsx_element_end_token>());
		CHECK(tokens[4]->to_string() == ">");

		// Hello
		CHECK(tokens[5].is<tscc::lex::tokens::jsx_text_token>());
		CHECK(tokens[5]->to_string() == "Hello");

		// </div>
		CHECK(tokens[6].is<tscc::lex::tokens::jsx_element_close_token>());
		CHECK(tokens[6]->to_string() == "</div>");
	}

	SECTION("Self-Closing JSX Element") {
		// <input />
		auto tokens = tokenize("<input />");
		REQUIRE(tokens.size() == 2);  // <input, />

		// <input
		CHECK(tokens[0].is<tscc::lex::tokens::jsx_element_start_token>());
		CHECK(tokens[0]->to_string() == "<input");

		// />
		CHECK(tokens[1].is<tscc::lex::tokens::jsx_self_closing_token>());
		CHECK(tokens[1]->to_string() == "/>");
	}

	SECTION("JSX Element with String Attribute") {
		// <div className="container">Text</div>
		auto tokens = tokenize(R"(<div className="container">Text</div>)");
		REQUIRE(tokens.size() ==
				6);	 // <div, className, ", "container", ", >, Text, </div>

		// <div
		CHECK(tokens[0].is<tscc::lex::tokens::jsx_element_start_token>());
		CHECK(tokens[0]->to_string() == "<div");

		// className
		CHECK(tokens[1].is<tscc::lex::tokens::jsx_attribute_name_token>());
		CHECK(tokens[1]->to_string() == "className");

		// ="container" (start) - properly check type
		REQUIRE(
			tokens[2].is<tscc::lex::tokens::jsx_attribute_value_token>());
		CHECK(tokens[2]->to_string() == "\"container\"");

		// >
		CHECK(tokens[3].is<tscc::lex::tokens::jsx_element_end_token>());
		CHECK(tokens[3]->to_string() == ">");

		// Text
		CHECK(tokens[4].is<tscc::lex::tokens::jsx_text_token>());
		CHECK(tokens[4]->to_string() == "Text");

		// </div>
		CHECK(tokens[5].is<tscc::lex::tokens::jsx_element_close_token>());
		CHECK(tokens[5]->to_string() == "</div>");
	}

	SECTION("JSX Element with Flag Attribute") {
		// <div className="container">Text</div>
		auto tokens =
			tokenize(R"(<div disabled className="container">Text</div>)");
		REQUIRE(tokens.size() ==
				7);	 // <div, className, ", "container", ", >, Text, </div>

		// <div
		CHECK(tokens[0].is<tscc::lex::tokens::jsx_element_start_token>());
		CHECK(tokens[0]->to_string() == "<div");

		// disabled
		CHECK(tokens[1].is<tscc::lex::tokens::jsx_attribute_name_token>());
		CHECK(tokens[1]->to_string() == "disabled");

		// className
		CHECK(tokens[2].is<tscc::lex::tokens::jsx_attribute_name_token>());
		CHECK(tokens[2]->to_string() == "className");

		// ="container" (start) - properly check type
		REQUIRE(
			tokens[3].is<tscc::lex::tokens::jsx_attribute_value_token>());
		CHECK(tokens[3]->to_string() == "\"container\"");

		// >
		CHECK(tokens[4].is<tscc::lex::tokens::jsx_element_end_token>());
		CHECK(tokens[4]->to_string() == ">");

		// Text
		CHECK(tokens[5].is<tscc::lex::tokens::jsx_text_token>());
		CHECK(tokens[5]->to_string() == "Text");

		// </div>
		CHECK(tokens[6].is<tscc::lex::tokens::jsx_element_close_token>());
		CHECK(tokens[6]->to_string() == "</div>");
	}

	SECTION("JSX Element with Expression Attribute") {
		// <button onClick={handleClick}>Click</button>
		auto tokens =
			tokenize("<button onClick={handleClick}>Click</button>");
		REQUIRE(
			tokens.size() ==
			8);	 // <button, onClick, {, handleClick, }, >, Click, </button>

		// <button
		CHECK(tokens[0].is<tscc::lex::tokens::jsx_element_start_token>());
		CHECK(tokens[0]->to_string() == "<button");

		// onClick
		CHECK(tokens[1].is<tscc::lex::tokens::jsx_attribute_name_token>());
		CHECK(tokens[1]->to_string() == "onClick");

		// ={handleClick} (start) - properly check type
		REQUIRE(
			tokens[2]
				.is<tscc::lex::tokens::jsx_attribute_value_start_token>());

		// handleClick (identifier inside expression)
		CHECK(tokens[3].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[3]->to_string() == "handleClick");

		// } (end) - properly check type
		REQUIRE(
			tokens[4]
				.is<tscc::lex::tokens::jsx_attribute_value_end_token>());

		// >
		CHECK(tokens[5].is<tscc::lex::tokens::jsx_element_end_token>());
		CHECK(tokens[5]->to_string() == ">");

		// Click
		CHECK(tokens[6].is<tscc::lex::tokens::jsx_text_token>());
		CHECK(tokens[6]->to_string() == "Click");

		// </button>
		CHECK(tokens[7].is<tscc::lex::tokens::jsx_element_close_token>());
		CHECK(tokens[7]->to_string() == "</button>");
	}

	SECTION("JSX Element with Template Literal Expression") {
		// <div className={`${base} ${modifier}`}>Content</div>
		auto tokens = tokenize(
			R"(<div className={`${base} ${modifier}`}>Content</div>)");
		REQUIRE(tokens.size() ==
				16);  // <div, className, {, `, ${, base, }, space, ${,
					  // modifier, }, `, }, >, Content, </div>

		// <div
		CHECK(tokens[0].is<tscc::lex::tokens::jsx_element_start_token>());

		// className
		CHECK(tokens[1].is<tscc::lex::tokens::jsx_attribute_name_token>());

		// ={
		REQUIRE(
			tokens[2]
				.is<tscc::lex::tokens::jsx_attribute_value_start_token>());

		// Template literal tokens: `, ${, base, }, space, ${, modifier, },
		// `
		CHECK(
			tokens[3]
				.is<tscc::lex::tokens::interpolated_string_start_token>());
		CHECK(tokens[4].is<tscc::lex::tokens::template_start_token>());
		CHECK(tokens[5].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[5]->to_string() == "base");
		CHECK(tokens[6].is<tscc::lex::tokens::template_end_token>());
		CHECK(
			tokens[7]
				.is<tscc::lex::tokens::interpolated_string_chunk_token>());
		CHECK(tokens[7]->to_string() == " ");
		CHECK(tokens[8].is<tscc::lex::tokens::template_start_token>());
		CHECK(tokens[9].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[9]->to_string() == "modifier");
		CHECK(tokens[10].is<tscc::lex::tokens::template_end_token>());
		CHECK(tokens[11]
				  .is<tscc::lex::tokens::interpolated_string_end_token>());

		// }
		REQUIRE(
			tokens[12]
				.is<tscc::lex::tokens::jsx_attribute_value_end_token>());

		// >
		CHECK(tokens[13].is<tscc::lex::tokens::jsx_element_end_token>());

		// Content
		CHECK(tokens[14].is<tscc::lex::tokens::jsx_text_token>());
		CHECK(tokens[14]->to_string() == "Content");

		// </div>
		CHECK(tokens[15].is<tscc::lex::tokens::jsx_element_close_token>());
		CHECK(tokens[15]->to_string() == "</div>");
	}

	SECTION("JSX with Multiple String Attributes") {
		// <input type="text" value="default" />
		auto tokens = tokenize(R"(<input type="text" value="default" />)");
		REQUIRE(
			tokens.size() ==
			6);	 // <input, type, ", "text", ", value, ", "default", ", />

		// <input
		CHECK(tokens[0].is<tscc::lex::tokens::jsx_element_start_token>());
		CHECK(tokens[0]->to_string() == "<input");

		// type="text"
		CHECK(tokens[1].is<tscc::lex::tokens::jsx_attribute_name_token>());
		CHECK(tokens[1]->to_string() == "type");

		REQUIRE(
			tokens[2].is<tscc::lex::tokens::jsx_attribute_value_token>());
		CHECK(tokens[2]->to_string() == "\"text\"");

		// value="default"
		CHECK(tokens[3].is<tscc::lex::tokens::jsx_attribute_name_token>());
		CHECK(tokens[3]->to_string() == "value");

		REQUIRE(
			tokens[4].is<tscc::lex::tokens::jsx_attribute_value_token>());
		CHECK(tokens[4]->to_string() == "\"default\"");

		// />
		CHECK(tokens[5].is<tscc::lex::tokens::jsx_self_closing_token>());
		CHECK(tokens[5]->to_string() == "/>");
	}

	SECTION("Nested JSX Elements") {
		// <div><span>Nested</span></div>
		auto tokens = tokenize("<div><span>Nested</span></div>");
		REQUIRE(tokens.size() ==
				7);	 // <div, >, <span, >, Nested, </span>, </div>

		// <div>
		CHECK(tokens[0].is<tscc::lex::tokens::jsx_element_start_token>());
		CHECK(tokens[0]->to_string() == "<div");
		CHECK(tokens[1].is<tscc::lex::tokens::jsx_element_end_token>());

		// <span>
		CHECK(tokens[2].is<tscc::lex::tokens::jsx_element_start_token>());
		CHECK(tokens[2]->to_string() == "<span");
		CHECK(tokens[3].is<tscc::lex::tokens::jsx_element_end_token>());

		// Nested
		CHECK(tokens[4].is<tscc::lex::tokens::jsx_text_token>());
		CHECK(tokens[4]->to_string() == "Nested");

		// </span>
		CHECK(tokens[5].is<tscc::lex::tokens::jsx_element_close_token>());
		CHECK(tokens[5]->to_string() == "</span>");

		// </div>
		CHECK(tokens[6].is<tscc::lex::tokens::jsx_element_close_token>());
		CHECK(tokens[6]->to_string() == "</div>");
	}

	SECTION("JSX with Mixed Content") {
		// <div>Text {variable} more text</div>
		auto tokens = tokenize("<div>Text {variable} more text</div>");
		REQUIRE(tokens.size() ==
				8);	 // <div, >, Text, {, variable, }, more text, </div>

		// <div>
		CHECK(tokens[0].is<tscc::lex::tokens::jsx_element_start_token>());
		CHECK(tokens[1].is<tscc::lex::tokens::jsx_element_end_token>());

		// Text
		CHECK(tokens[2].is<tscc::lex::tokens::jsx_text_token>());
		CHECK(tokens[2]->to_string() == "Text ");

		// {variable}
		REQUIRE(tokens[3].is<tscc::lex::tokens::template_start_token>());
		CHECK(tokens[3]->to_string() == "{");

		CHECK(tokens[4].is<tscc::lex::tokens::identifier_token>());
		CHECK(tokens[4]->to_string() == "variable");

		REQUIRE(tokens[5].is<tscc::lex::tokens::template_end_token>());
		CHECK(tokens[5]->to_string() == "}");

		// more text
		CHECK(tokens[6].is<tscc::lex::tokens::jsx_text_token>());
		CHECK(tokens[6]->to_string() == " more text");

		// </div>
		CHECK(tokens[7].is<tscc::lex::tokens::jsx_element_close_token>());
	}

	SECTION("JSX Fragment") {
		// <>Fragment content</>
		auto tokens = tokenize("<>Fragment content</>");
		REQUIRE(tokens.size() == 4);  // <, >, Fragment content, </>

		// <> (React Fragment)
		CHECK(tokens[0].is<tscc::lex::tokens::jsx_element_start_token>());
		CHECK(tokens[0]->to_string() == "<");
		CHECK(tokens[1].is<tscc::lex::tokens::jsx_element_end_token>());

		// Fragment content
		CHECK(tokens[2].is<tscc::lex::tokens::jsx_text_token>());
		CHECK(tokens[2]->to_string() == "Fragment content");

		// </>
		CHECK(tokens[3].is<tscc::lex::tokens::jsx_element_close_token>());
		CHECK(tokens[3]->to_string() == "</>");
	}

	SECTION("JSX with Unicode Element Names") {
		// <MyComponent π="3.14">Content</MyComponent>
		auto tokens =
			tokenize("<MyComponent ℼ=\"3.14\">Content</MyComponent>");
		REQUIRE(tokens.size() == 6);  // <MyComponent, ℼ, ", "3.14", ", >,
									  // Content, </MyComponent>

		// <MyComponent
		CHECK(tokens[0].is<tscc::lex::tokens::jsx_element_start_token>());
		CHECK(tokens[0]->to_string() == "<MyComponent");

		// π (Unicode attribute name)
		CHECK(tokens[1].is<tscc::lex::tokens::jsx_attribute_name_token>());
		CHECK(tokens[1]->to_string() == "ℼ");

		// ="3.14"
		REQUIRE(
			tokens[2].is<tscc::lex::tokens::jsx_attribute_value_token>());
		CHECK(tokens[2]->to_string() == "\"3.14\"");

		// >
		CHECK(tokens[3].is<tscc::lex::tokens::jsx_element_end_token>());

		// Content
		CHECK(tokens[4].is<tscc::lex::tokens::jsx_text_token>());
		CHECK(tokens[4]->to_string() == "Content");

		// </MyComponent>
		CHECK(tokens[5].is<tscc::lex::tokens::jsx_element_close_token>());
		CHECK(tokens[5]->to_string() == "</MyComponent>");
	}

	SECTION("JSX Error Cases") {
		SECTION("Unterminated JSX Element") {
			auto lexer = create_lexer("<div>Unterminated");
			REQUIRE_THROWS(
				std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
		}

		SECTION("Unterminated JSX Attribute String") {
			auto lexer = create_lexer(R"(<div className="unterminated>)");
			REQUIRE_THROWS(
				std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
		}

		SECTION("Unterminated JSX Attribute Expression") {
			auto lexer = create_lexer("<div onClick={unterminated");
			REQUIRE_THROWS(
				std::vector<tscc::lex::token>{lexer.begin(), lexer.end()});
		}
	}
}