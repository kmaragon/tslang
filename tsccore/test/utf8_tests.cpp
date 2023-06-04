//
// Created by minh on 5/25/23.
//

#include <catch2/catch_test_macros.hpp>
#include "tsccore/utf8.hpp"

#ifndef __FILE_NAME__
#  define __FILE_NAME__ __FILE__
#endif

TEST_CASE("UTF-8 Encoding")
{
	SECTION("string in ASCII")
	{
		std::u32string s = U"this is in ascii";
		std::string res = tscc::utf8_encode(s);
		REQUIRE(res == "this is in ascii");
	}

	SECTION("char in string requires 2 bytes")
	{
		std::u32string s = U"A©";
		std::string res = tscc::utf8_encode(s);
		REQUIRE(res == "A©");
	}

	SECTION("char in string requires 3 bytes")
	{
		std::u32string s = U"汉";
		std::string res = tscc::utf8_encode(s);
		REQUIRE(res == "汉");
	}

	SECTION("character in UTF-8 requires 4 bytes formatting")
	{
		std::u32string s = U"\u1F600";
		std::string res = tscc::utf8_encode(s);
		REQUIRE(res == "\u1F600");
	}

	SECTION("characters in UTF-8 requires 2, 3 and 4 byte formatting")
	{
		std::u32string s = U"¢汉\u1F600";
		std::string res = tscc::utf8_encode(s);
		REQUIRE(res == "¢汉\u1F600");
	}
}

TEST_CASE("UTF-8 Size Requirements", "encoding sizes")
{
	SECTION("empty string requires 0 bytes")
	{
		std::u32string s = U"";
		std::size_t size = tscc::utf8_size(s);
		REQUIRE(size == s.size());
	}

	SECTION("string in ASCII require 1 byte each character")
	{
		std::u32string s = U"this is in ascii";
		std::size_t size = tscc::utf8_size(s);
		REQUIRE(size == s.size());
	}

	SECTION("character in UTF-8 requires 2 bytes formatting")
	{
		std::u32string s = U"¢¢";
		std::size_t size = tscc::utf8_size(s);
		REQUIRE(size == (2 + 2));
	}

	SECTION("character in UTF-8 requires 3 bytes formatting")
	{
		std::u32string s = U"汉汉";
		std::size_t size = tscc::utf8_size(s);
		REQUIRE(size == (3 + 3));
	}

	SECTION("character in UTF-8 requires 4 bytes formatting")
	{
		std::u32string s = U"\u1F600";
		std::size_t size = tscc::utf8_size(s);
		REQUIRE(size == 4);
	}

	SECTION("characters in UTF-8 requires 2, 3 and 4 byte formatting")
	{
		std::u32string s = U"¢汉\u1F600";
		std::size_t size = tscc::utf8_size(s);
		REQUIRE(size == (2 + 3 + 4));
	}
}
