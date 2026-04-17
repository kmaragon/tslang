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

#include "xml.hpp"
#include <format>

namespace {

bool is_valid_xml_char(char32_t cp) {
	return cp == 0x9 || cp == 0xA || cp == 0xD ||
		   (cp >= 0x20 && cp <= 0xD7FF) ||
		   (cp >= 0xE000 && cp <= 0xFFFD) ||
		   (cp >= 0x10000 && cp <= 0x10FFFF);
}

}  // namespace

tscc::invalid_xml_character::invalid_xml_character(
	char32_t codepoint, std::size_t position)
	: std::runtime_error(
		  std::format("Invalid XML character U+{:04X} at position {}",
					  static_cast<uint32_t>(codepoint), position)),
	  codepoint_(codepoint),
	  position_(position) {}

std::u32string tscc::xml_decode(const std::u32string_view& input,
								xml_invalid_char policy) {
	std::u32string result;
	result.reserve(input.size());

	for (size_t i = 0; i < input.size(); ++i) {
		if (input[i] != U'&') {
			result.push_back(input[i]);
			continue;
		}

		auto semi = input.find(U';', i + 1);
		if (semi == std::u32string_view::npos) {
			result.push_back(input[i]);
			continue;
		}

		auto ref = input.substr(i + 1, semi - i - 1);

		if (ref == U"amp")       result.push_back(U'&');
		else if (ref == U"lt")   result.push_back(U'<');
		else if (ref == U"gt")   result.push_back(U'>');
		else if (ref == U"quot") result.push_back(U'"');
		else if (ref == U"apos") result.push_back(U'\'');
		else if (ref.size() >= 2 && ref[0] == U'#') {
			char32_t cp = 0;
			bool valid = true;

			if (ref[1] == U'x' || ref[1] == U'X') {
				for (size_t j = 2; j < ref.size(); ++j) {
					auto c = ref[j];
					if (c >= U'0' && c <= U'9')
						cp = cp * 16 + (c - U'0');
					else if (c >= U'a' && c <= U'f')
						cp = cp * 16 + (c - U'a' + 10);
					else if (c >= U'A' && c <= U'F')
						cp = cp * 16 + (c - U'A' + 10);
					else { valid = false; break; }
				}
				if (ref.size() < 3) valid = false;
			} else {
				for (size_t j = 1; j < ref.size(); ++j) {
					auto c = ref[j];
					if (c >= U'0' && c <= U'9')
						cp = cp * 10 + (c - U'0');
					else { valid = false; break; }
				}
			}

			if (valid && is_valid_xml_char(cp)) {
				result.push_back(cp);
			} else if (valid && !is_valid_xml_char(cp)) {
				if (policy == xml_invalid_char::throw_exception)
					throw invalid_xml_character(cp, i);
				result.append(input.substr(i, semi - i + 1));
			} else {
				result.append(input.substr(i, semi - i + 1));
			}
		} else {
			result.push_back(input[i]);
			continue;
		}

		i = semi;
	}

	result.shrink_to_fit();
	return result;
}

std::u32string tscc::xml_encode(const std::u32string_view& input,
								xml_invalid_char policy) {
	std::u32string result;
	result.reserve(input.size() * 3);

	for (size_t i = 0; i < input.size(); ++i) {
		auto c = input[i];
		switch (c) {
			case U'&':  result.append(U"&amp;");  break;
			case U'<':  result.append(U"&lt;");   break;
			case U'>':  result.append(U"&gt;");   break;
			case U'"':  result.append(U"&quot;"); break;
			case U'\'': result.append(U"&apos;"); break;
			default:
				if (is_valid_xml_char(c)) {
					result.push_back(c);
				} else if (policy == xml_invalid_char::throw_exception) {
					throw invalid_xml_character(c, i);
				}
				break;
		}
	}

	result.shrink_to_fit();
	return result;
}
