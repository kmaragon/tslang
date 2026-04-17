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
#include "tsccore/xml.hpp"

TEST_CASE("xml_encode", "[xml]") {
	SECTION("empty string") {
		REQUIRE(tscc::xml_encode(U"") == U"");
	}

	SECTION("no special characters") {
		REQUIRE(tscc::xml_encode(U"hello world") == U"hello world");
	}

	SECTION("ampersand") {
		REQUIRE(tscc::xml_encode(U"a&b") == U"a&amp;b");
	}

	SECTION("less than") {
		REQUIRE(tscc::xml_encode(U"a<b") == U"a&lt;b");
	}

	SECTION("greater than") {
		REQUIRE(tscc::xml_encode(U"a>b") == U"a&gt;b");
	}

	SECTION("double quote") {
		REQUIRE(tscc::xml_encode(U"a\"b") == U"a&quot;b");
	}

	SECTION("single quote") {
		REQUIRE(tscc::xml_encode(U"a'b") == U"a&apos;b");
	}

	SECTION("all five special characters") {
		REQUIRE(tscc::xml_encode(U"<div class=\"a&b\">it's</div>")
				== U"&lt;div class=&quot;a&amp;b&quot;&gt;it&apos;s&lt;/div&gt;");
	}

	SECTION("only special characters") {
		REQUIRE(tscc::xml_encode(U"<>&\"'") ==
				U"&lt;&gt;&amp;&quot;&apos;");
	}

	SECTION("consecutive ampersands") {
		REQUIRE(tscc::xml_encode(U"&&&&") ==
				U"&amp;&amp;&amp;&amp;");
	}

	SECTION("kanji passes through unchanged") {
		REQUIRE(tscc::xml_encode(U"漢字テスト") == U"漢字テスト");
	}

	SECTION("kanji mixed with special characters") {
		REQUIRE(tscc::xml_encode(U"<漢字>") == U"&lt;漢字&gt;");
	}

	SECTION("emoji passes through unchanged") {
		std::u32string emoji = U"\U0001F600\U0001F4A9\U0001F680";
		REQUIRE(tscc::xml_encode(emoji) == emoji);
	}

	SECTION("emoji mixed with special characters") {
		REQUIRE(tscc::xml_encode(U"<\U0001F600>&\U0001F4A9")
				== U"&lt;\U0001F600&gt;&amp;\U0001F4A9");
	}

	SECTION("existing entity-like text gets double-encoded") {
		REQUIRE(tscc::xml_encode(U"&amp;") == U"&amp;amp;");
		REQUIRE(tscc::xml_encode(U"&lt;") == U"&amp;lt;");
	}

	SECTION("invalid XML characters throw by default") {
		SECTION("surrogate throws") {
			std::u32string input = {U'a', char32_t(0xD800), U'b'};
			REQUIRE_THROWS_AS(tscc::xml_encode(input),
							  tscc::invalid_xml_character);
		}

		SECTION("exception carries codepoint and position") {
			std::u32string input = {U'a', U'b', char32_t(0xDC00)};
			try {
				tscc::xml_encode(input);
				REQUIRE(false);
			} catch (const tscc::invalid_xml_character& e) {
				REQUIRE(e.codepoint() == char32_t(0xDC00));
				REQUIRE(e.position() == 2);
			}
		}

		SECTION("null throws") {
			std::u32string input = {char32_t(0x0000)};
			REQUIRE_THROWS_AS(tscc::xml_encode(input),
							  tscc::invalid_xml_character);
		}

		SECTION("FFFE throws") {
			std::u32string input = {char32_t(0xFFFE)};
			REQUIRE_THROWS_AS(tscc::xml_encode(input),
							  tscc::invalid_xml_character);
		}

		SECTION("FFFF throws") {
			std::u32string input = {char32_t(0xFFFF)};
			REQUIRE_THROWS_AS(tscc::xml_encode(input),
							  tscc::invalid_xml_character);
		}
	}

	SECTION("invalid XML characters dropped with drop policy") {
		using enum tscc::xml_invalid_char;

		SECTION("surrogates are dropped") {
			std::u32string input = {U'a', char32_t(0xD800), U'b'};
			REQUIRE(tscc::xml_encode(input, drop) == U"ab");
		}

		SECTION("all surrogate types are dropped") {
			std::u32string input = {char32_t(0xD800), char32_t(0xDBFF),
									char32_t(0xDC00), char32_t(0xDFFF)};
			REQUIRE(tscc::xml_encode(input, drop) == U"");
		}

		SECTION("surrogates dropped with surrounding specials preserved") {
			std::u32string input = {U'<', char32_t(0xD800), U'>'};
			REQUIRE(tscc::xml_encode(input, drop) == U"&lt;&gt;");
		}

		SECTION("FFFE and FFFF are dropped") {
			std::u32string input = {char32_t(0xFFFE), char32_t(0xFFFF)};
			REQUIRE(tscc::xml_encode(input, drop) == U"");
		}

		SECTION("null is dropped") {
			std::u32string input = {char32_t(0x0000)};
			REQUIRE(tscc::xml_encode(input, drop) == U"");
		}
	}
}

TEST_CASE("xml_decode", "[xml]") {
	SECTION("empty string") {
		REQUIRE(tscc::xml_decode(U"") == U"");
	}

	SECTION("no entities") {
		REQUIRE(tscc::xml_decode(U"hello world") == U"hello world");
	}

	SECTION("named entities") {
		SECTION("amp") {
			REQUIRE(tscc::xml_decode(U"a&amp;b") == U"a&b");
		}

		SECTION("lt") {
			REQUIRE(tscc::xml_decode(U"a&lt;b") == U"a<b");
		}

		SECTION("gt") {
			REQUIRE(tscc::xml_decode(U"a&gt;b") == U"a>b");
		}

		SECTION("quot") {
			REQUIRE(tscc::xml_decode(U"a&quot;b") == U"a\"b");
		}

		SECTION("apos") {
			REQUIRE(tscc::xml_decode(U"a&apos;b") == U"a'b");
		}

		SECTION("all five in context") {
			REQUIRE(tscc::xml_decode(
				U"&lt;div class=&quot;a&amp;b&quot;&gt;it&apos;s&lt;/div&gt;")
				== U"<div class=\"a&b\">it's</div>");
		}
	}

	SECTION("decimal numeric references") {
		SECTION("ASCII character") {
			REQUIRE(tscc::xml_decode(U"&#65;") == U"A");
		}

		SECTION("copyright sign") {
			REQUIRE(tscc::xml_decode(U"&#169;") == U"\u00A9");
		}

		SECTION("kanji via decimal") {
			REQUIRE(tscc::xml_decode(U"&#28450;") == U"漢");
		}

		SECTION("emoji via decimal") {
			REQUIRE(tscc::xml_decode(U"&#128512;") ==
					std::u32string(1, U'\U0001F600'));
		}
	}

	SECTION("hex numeric references") {
		SECTION("lowercase hex") {
			REQUIRE(tscc::xml_decode(U"&#x41;") == U"A");
		}

		SECTION("uppercase hex") {
			REQUIRE(tscc::xml_decode(U"&#X41;") == U"A");
		}

		SECTION("mixed case hex digits") {
			REQUIRE(tscc::xml_decode(U"&#x1f600;") ==
					std::u32string(1, U'\U0001F600'));
		}

		SECTION("kanji via hex") {
			REQUIRE(tscc::xml_decode(U"&#x6F22;") == U"漢");
		}
	}

	SECTION("malformed entities pass through") {
		SECTION("ampersand without semicolon at end") {
			REQUIRE(tscc::xml_decode(U"trailing &") == U"trailing &");
		}

		SECTION("ampersand without semicolon mid-string") {
			REQUIRE(tscc::xml_decode(U"a & b") == U"a & b");
		}

		SECTION("unknown named entity") {
			REQUIRE(tscc::xml_decode(U"&bogus;") == U"&bogus;");
		}

		SECTION("empty entity reference") {
			REQUIRE(tscc::xml_decode(U"&;") == U"&;");
		}

		SECTION("numeric reference with no digits") {
			REQUIRE(tscc::xml_decode(U"&#;") == U"&#;");
		}

		SECTION("hex reference with no digits") {
			REQUIRE(tscc::xml_decode(U"&#x;") == U"&#x;");
		}

		SECTION("null character reference throws by default") {
			REQUIRE_THROWS_AS(tscc::xml_decode(U"&#0;"),
							  tscc::invalid_xml_character);
			REQUIRE_THROWS_AS(tscc::xml_decode(U"&#x0;"),
							  tscc::invalid_xml_character);
		}

		SECTION("null character reference passes through with drop") {
			using enum tscc::xml_invalid_char;
			REQUIRE(tscc::xml_decode(U"&#0;", drop) == U"&#0;");
			REQUIRE(tscc::xml_decode(U"&#x0;", drop) == U"&#x0;");
		}

		SECTION("surrogate numeric references throw by default") {
			REQUIRE_THROWS_AS(tscc::xml_decode(U"&#xD800;"),
							  tscc::invalid_xml_character);
			REQUIRE_THROWS_AS(tscc::xml_decode(U"&#xDFFF;"),
							  tscc::invalid_xml_character);
			REQUIRE_THROWS_AS(tscc::xml_decode(U"&#55296;"),
							  tscc::invalid_xml_character);
		}

		SECTION("surrogate numeric references pass through with drop") {
			using enum tscc::xml_invalid_char;
			REQUIRE(tscc::xml_decode(U"&#xD800;", drop) == U"&#xD800;");
			REQUIRE(tscc::xml_decode(U"&#xDFFF;", drop) == U"&#xDFFF;");
			REQUIRE(tscc::xml_decode(U"&#xDBFF;", drop) == U"&#xDBFF;");
			REQUIRE(tscc::xml_decode(U"&#xDC00;", drop) == U"&#xDC00;");
			REQUIRE(tscc::xml_decode(U"&#55296;", drop) == U"&#55296;");
		}

		SECTION("decode exception carries codepoint and position") {
			try {
				tscc::xml_decode(U"ok&#xD800;after");
				REQUIRE(false);
			} catch (const tscc::invalid_xml_character& e) {
				REQUIRE(e.codepoint() == char32_t(0xD800));
				REQUIRE(e.position() == 2);
			}
		}

		SECTION("invalid hex digit") {
			REQUIRE(tscc::xml_decode(U"&#xZZ;") == U"&#xZZ;");
		}

		SECTION("invalid decimal digit") {
			REQUIRE(tscc::xml_decode(U"&#12abc;") == U"&#12abc;");
		}
	}

	SECTION("kanji and emoji pass through unchanged") {
		std::u32string text = U"漢字テスト\U0001F600\U0001F680";
		REQUIRE(tscc::xml_decode(text) == text);
	}

	SECTION("entities adjacent to kanji") {
		REQUIRE(tscc::xml_decode(U"&lt;漢字&gt;") == U"<漢字>");
	}

	SECTION("entities adjacent to emoji") {
		REQUIRE(tscc::xml_decode(U"&amp;\U0001F600&amp;") ==
				U"&\U0001F600&");
	}

	SECTION("consecutive entities") {
		REQUIRE(tscc::xml_decode(U"&amp;&amp;&amp;") == U"&&&");
		REQUIRE(tscc::xml_decode(U"&lt;&gt;") == U"<>");
	}

	SECTION("double-encoded entities decode one layer") {
		REQUIRE(tscc::xml_decode(U"&amp;lt;") == U"&lt;");
		REQUIRE(tscc::xml_decode(U"&amp;amp;") == U"&amp;");
	}
}

TEST_CASE("xml round-trip", "[xml]") {
	SECTION("plain ASCII") {
		auto input = U"hello world 123";
		REQUIRE(tscc::xml_decode(tscc::xml_encode(input)) == input);
	}

	SECTION("special characters") {
		auto input = U"<div class=\"test\">&'foo'</div>";
		REQUIRE(tscc::xml_decode(tscc::xml_encode(input)) == input);
	}

	SECTION("kanji") {
		auto input = U"東京タワー<名所>";
		REQUIRE(tscc::xml_decode(tscc::xml_encode(input)) == input);
	}

	SECTION("emoji") {
		auto input = U"\U0001F600 < \U0001F4A9 & \U0001F680";
		REQUIRE(tscc::xml_decode(tscc::xml_encode(input)) == input);
	}

	SECTION("mixed CJK and special characters") {
		auto input = U"<漢字 attr=\"値\">&テスト's</漢字>";
		REQUIRE(tscc::xml_decode(tscc::xml_encode(input)) == input);
	}
}
