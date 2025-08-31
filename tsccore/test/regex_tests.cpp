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

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include "../tsccore/regex/alternative.hpp"
#include "../tsccore/regex/assertion.hpp"
#include "../tsccore/regex/atom.hpp"
#include "../tsccore/regex/character_class.hpp"
#include "../tsccore/regex/error.hpp"
#include "../tsccore/regex/quantifier.hpp"
#include "../tsccore/regex/regular_expression.hpp"
#include "../tsccore/regex/scan_regex.hpp"

using namespace tsccore::regex;


TEST_CASE("Regular Expression Scanning", "[regex]") {
	SECTION("Simple character") {
		std::u32string input = U"a";
		regular_expression regex;

		REQUIRE_NOTHROW(scan(input, regex));

		const auto& disjunction = regex.get_disjunction();
		auto alternatives = disjunction.get_alternatives();
		REQUIRE(alternatives.size() == 1);

		const auto& terms = alternatives[0].get_terms();
		REQUIRE(terms.size() == 1);
		REQUIRE(!terms[0].is_assertion());

		const auto& atom = terms[0].get_atom();
		REQUIRE(atom.is_character());
		REQUIRE(atom.get_character() == U'a');
	}

	SECTION("Character classes") {
		SECTION("Basic character class") {
			std::u32string input = U"[abc]";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& disjunction = regex.get_disjunction();
			auto alternatives = disjunction.get_alternatives();
			REQUIRE(alternatives.size() == 1);

			const auto& terms = alternatives[0].get_terms();
			REQUIRE(terms.size() == 1);

			const auto& atom = terms[0].get_atom();
			REQUIRE(atom.is_character_class());

			const auto& char_class = atom.get_character_class();
			REQUIRE(!char_class.is_negated());

			const auto& characters = char_class.get_characters();
			REQUIRE(characters.count(U'a') == 1);
			REQUIRE(characters.count(U'b') == 1);
			REQUIRE(characters.count(U'c') == 1);
		}

		SECTION("Negated character class") {
			std::u32string input = U"[^abc]";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& disjunction = regex.get_disjunction();
			auto alternatives = disjunction.get_alternatives();
			REQUIRE(alternatives.size() == 1);

			const auto& terms = alternatives[0].get_terms();
			REQUIRE(terms.size() == 1);

			const auto& atom = terms[0].get_atom();
			REQUIRE(atom.is_character_class());

			const auto& char_class = atom.get_character_class();
			REQUIRE(char_class.is_negated());
		}

		SECTION("Character range") {
			std::u32string input = U"[a-z]";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& disjunction = regex.get_disjunction();
			auto alternatives = disjunction.get_alternatives();
			REQUIRE(alternatives.size() == 1);

			const auto& terms = alternatives[0].get_terms();
			REQUIRE(terms.size() == 1);

			const auto& atom = terms[0].get_atom();
			REQUIRE(atom.is_character_class());

			const auto& char_class = atom.get_character_class();
			const auto& ranges = char_class.get_ranges();
			REQUIRE(ranges.size() == 1);
			REQUIRE(ranges[0].first == U'a');
			REQUIRE(ranges[0].second == U'z');
		}

		SECTION("Character class with escaped characters") {
			std::u32string input = U"[\\]]";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character_class());

			const auto& char_class = atom.get_character_class();
			const auto& characters = char_class.get_characters();
			REQUIRE(characters.count(U']') == 1);
		}

		SECTION("Character class with dash at end") {
			std::u32string input = U"[abc-]";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character_class());

			const auto& char_class = atom.get_character_class();
			const auto& characters = char_class.get_characters();
			REQUIRE(characters.count(U'a') == 1);
			REQUIRE(characters.count(U'b') == 1);
			REQUIRE(characters.count(U'c') == 1);
			REQUIRE(characters.count(U'-') == 1);
		}

		SECTION("Mixed ranges and characters") {
			std::u32string input = U"[a-z0-9_]";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character_class());

			const auto& char_class = atom.get_character_class();
			const auto& ranges = char_class.get_ranges();
			REQUIRE(ranges.size() == 2);
			REQUIRE(ranges[0].first == U'a');
			REQUIRE(ranges[0].second == U'z');
			REQUIRE(ranges[1].first == U'0');
			REQUIRE(ranges[1].second == U'9');

			const auto& characters = char_class.get_characters();
			REQUIRE(characters.count(U'_') == 1);
		}

		SECTION("Character class with escape sequences") {
			std::u32string input = U"[\\n\\t]";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character_class());

			const auto& char_class = atom.get_character_class();
			const auto& characters = char_class.get_characters();
			REQUIRE(characters.count(U'\n') == 1);
			REQUIRE(characters.count(U'\t') == 1);
		}
	}

	SECTION("Quantifiers") {
		SECTION("Star quantifier") {
			std::u32string input = U"a*";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& disjunction = regex.get_disjunction();
			auto alternatives = disjunction.get_alternatives();
			REQUIRE(alternatives.size() == 1);

			const auto& terms = alternatives[0].get_terms();
			REQUIRE(terms.size() == 1);

			const auto& atom = terms[0].get_atom();
			REQUIRE(atom.is_character());
			REQUIRE(atom.get_character() == U'a');

			const auto& quantifier_opt = terms[0].get_quantifier();
			REQUIRE(quantifier_opt.has_value());

			const auto& quant = quantifier_opt.value();
			REQUIRE(quant.is_prefix());
			REQUIRE(quant.get_prefix() == quantifier::prefix::zero_or_more);
		}

		SECTION("Plus quantifier") {
			std::u32string input = U"a+";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& terms = regex.get_disjunction().get_alternatives()[0].get_terms();
			REQUIRE(terms.size() == 1);

			const auto& quantifier_opt = terms[0].get_quantifier();
			REQUIRE(quantifier_opt.has_value());

			const auto& quant = quantifier_opt.value();
			REQUIRE(quant.is_prefix());
			REQUIRE(quant.get_prefix() == quantifier::prefix::one_or_more);
		}

		SECTION("Question mark quantifier") {
			std::u32string input = U"a?";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& terms = regex.get_disjunction().get_alternatives()[0].get_terms();
			REQUIRE(terms.size() == 1);

			const auto& quantifier_opt = terms[0].get_quantifier();
			REQUIRE(quantifier_opt.has_value());

			const auto& quant = quantifier_opt.value();
			REQUIRE(quant.is_prefix());
			REQUIRE(quant.get_prefix() == quantifier::prefix::zero_or_one);
		}

		SECTION("Exact count quantifier") {
			std::u32string input = U"a{3}";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& terms = regex.get_disjunction().get_alternatives()[0].get_terms();
			REQUIRE(terms.size() == 1);

			const auto& quantifier_opt = terms[0].get_quantifier();
			REQUIRE(quantifier_opt.has_value());

			const auto& quant = quantifier_opt.value();
			REQUIRE(quant.is_range());
			REQUIRE(quant.get_range().first == 3);
			REQUIRE(quant.get_range().second == 3);
		}

		SECTION("Min count quantifier") {
			std::u32string input = U"a{3,}";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& terms = regex.get_disjunction().get_alternatives()[0].get_terms();
			REQUIRE(terms.size() == 1);

			const auto& quantifier_opt = terms[0].get_quantifier();
			REQUIRE(quantifier_opt.has_value());

			const auto& quant = quantifier_opt.value();
			REQUIRE(quant.is_range());
			REQUIRE(quant.get_range().first == 3);
			REQUIRE(quant.get_range().second == SIZE_MAX);
		}

		SECTION("Range quantifier") {
			std::u32string input = U"a{2,5}";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& terms = regex.get_disjunction().get_alternatives()[0].get_terms();
			REQUIRE(terms.size() == 1);

			const auto& quantifier_opt = terms[0].get_quantifier();
			REQUIRE(quantifier_opt.has_value());

			const auto& quant = quantifier_opt.value();
			REQUIRE(quant.is_range());
			REQUIRE(quant.get_range().first == 2);
			REQUIRE(quant.get_range().second == 5);
		}

		SECTION("Quantifiers on character classes") {
			std::u32string input = U"[abc]*";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& terms = regex.get_disjunction().get_alternatives()[0].get_terms();
			REQUIRE(terms.size() == 1);

			const auto& atom = terms[0].get_atom();
			REQUIRE(atom.is_character_class());

			const auto& quantifier_opt = terms[0].get_quantifier();
			REQUIRE(quantifier_opt.has_value());
			REQUIRE(quantifier_opt.value().get_prefix() == quantifier::prefix::zero_or_more);
		}
	}

	SECTION("Disjunctions") {
		SECTION("Simple disjunction") {
			std::u32string input = U"a|b";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& disjunction = regex.get_disjunction();
			auto alternatives = disjunction.get_alternatives();
			REQUIRE(alternatives.size() == 2);

			// First alternative: 'a'
			const auto& terms1 = alternatives[0].get_terms();
			REQUIRE(terms1.size() == 1);
			const auto& atom1 = terms1[0].get_atom();
			REQUIRE(atom1.is_character());
			REQUIRE(atom1.get_character() == U'a');

			// Second alternative: 'b'
			const auto& terms2 = alternatives[1].get_terms();
			REQUIRE(terms2.size() == 1);
			const auto& atom2 = terms2[0].get_atom();
			REQUIRE(atom2.is_character());
			REQUIRE(atom2.get_character() == U'b');
		}

		SECTION("Multiple alternatives") {
			std::u32string input = U"a|b|c";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& alternatives = regex.get_disjunction().get_alternatives();
			REQUIRE(alternatives.size() == 3);

			REQUIRE(alternatives[0].get_terms()[0].get_atom().get_character() == U'a');
			REQUIRE(alternatives[1].get_terms()[0].get_atom().get_character() == U'b');
			REQUIRE(alternatives[2].get_terms()[0].get_atom().get_character() == U'c');
		}

		SECTION("Complex disjunction with groups") {
			std::u32string input = U"(a|b)+c";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& terms = regex.get_disjunction().get_alternatives()[0].get_terms();
			REQUIRE(terms.size() == 2);

			const auto& group_atom = terms[0].get_atom();
			REQUIRE(group_atom.is_group());
			REQUIRE(terms[0].get_quantifier().has_value());

			const auto& c_atom = terms[1].get_atom();
			REQUIRE(c_atom.is_character());
			REQUIRE(c_atom.get_character() == U'c');
		}
	}

	SECTION("Complex patterns") {
		SECTION("Mixed quantifiers") {
			std::u32string input = U"a+b*c?";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& terms = regex.get_disjunction().get_alternatives()[0].get_terms();
			REQUIRE(terms.size() == 3);

			REQUIRE(terms[0].get_quantifier().value().get_prefix() == quantifier::prefix::one_or_more);
			REQUIRE(terms[1].get_quantifier().value().get_prefix() == quantifier::prefix::zero_or_more);
			REQUIRE(terms[2].get_quantifier().value().get_prefix() == quantifier::prefix::zero_or_one);
		}

		SECTION("Nested groups") {
			std::u32string input = U"((a))";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& outer_atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(outer_atom.is_group());

			const auto& inner_terms = outer_atom.get_group().get_disjunction().get_alternatives()[0].get_terms();
			REQUIRE(inner_terms.size() == 1);

			const auto& inner_atom = inner_terms[0].get_atom();
			REQUIRE(inner_atom.is_group());
		}
	}

	SECTION("Escape sequences") {
		SECTION("Newline escape") {
			std::u32string input = U"\\n";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character());
			REQUIRE(atom.get_character() == U'\n');
		}

		SECTION("Tab escape") {
			std::u32string input = U"\\t";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character());
			REQUIRE(atom.get_character() == U'\t');
		}

		SECTION("Carriage return escape") {
			std::u32string input = U"\\r";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character());
			REQUIRE(atom.get_character() == U'\r');
		}

		SECTION("Form feed escape") {
			std::u32string input = U"\\f";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character());
			REQUIRE(atom.get_character() == U'\f');
		}

		SECTION("Vertical tab escape") {
			std::u32string input = U"\\v";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character());
			REQUIRE(atom.get_character() == U'\v');
		}

		SECTION("Null escape") {
			std::u32string input = U"\\0";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character());
			REQUIRE(atom.get_character() == U'\0');
		}

		SECTION("Backslash escape") {
			std::u32string input = U"\\\\";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character());
			REQUIRE(atom.get_character() == U'\\');
		}

		SECTION("Dot escape") {
			std::u32string input = U"\\.";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character());
			REQUIRE(atom.get_character() == U'.');
		}

		SECTION("Star escape") {
			std::u32string input = U"\\*";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character());
			REQUIRE(atom.get_character() == U'*');
		}

		SECTION("Plus escape") {
			std::u32string input = U"\\+";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character());
			REQUIRE(atom.get_character() == U'+');
		}

		SECTION("Hex escape") {
			std::u32string input = U"\\x41";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character());
			REQUIRE(atom.get_character() == U'A');
		}

		SECTION("Unicode escape") {
			std::u32string input = U"\\u0041";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_character());
			REQUIRE(atom.get_character() == U'A');
		}
	}

	SECTION("Builtin character classes") {
		SECTION("Digit class") {
			std::u32string input = U"\\d";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& disjunction = regex.get_disjunction();
			auto alternatives = disjunction.get_alternatives();
			REQUIRE(alternatives.size() == 1);

			const auto& terms = alternatives[0].get_terms();
			REQUIRE(terms.size() == 1);

			const auto& atom = terms[0].get_atom();
			REQUIRE(atom.is_builtin_class());
			REQUIRE(atom.get_builtin_class() == atom::builtin_class::digit);
		}

		SECTION("Word class") {
			std::u32string input = U"\\w";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_builtin_class());
			REQUIRE(atom.get_builtin_class() == atom::builtin_class::word);
		}

		SECTION("Non-word class") {
			std::u32string input = U"\\W";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_builtin_class());
			REQUIRE(atom.get_builtin_class() == atom::builtin_class::non_word);
		}

		SECTION("Non-digit class") {
			std::u32string input = U"\\D";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_builtin_class());
			REQUIRE(atom.get_builtin_class() == atom::builtin_class::non_digit);
		}

		SECTION("Whitespace class") {
			std::u32string input = U"\\s";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_builtin_class());
			REQUIRE(atom.get_builtin_class() == atom::builtin_class::whitespace);
		}

		SECTION("Non-whitespace class") {
			std::u32string input = U"\\S";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_builtin_class());
			REQUIRE(atom.get_builtin_class() == atom::builtin_class::non_whitespace);
		}

		SECTION("Dot metacharacter") {
			std::u32string input = U".";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_builtin_class());
			REQUIRE(atom.get_builtin_class() == atom::builtin_class::dot);
		}
	}

	SECTION("Groups") {
		SECTION("Basic capturing group") {
			std::u32string input = U"(abc)";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& terms = regex.get_disjunction().get_alternatives()[0].get_terms();
			REQUIRE(terms.size() == 1);

			const auto& atom = terms[0].get_atom();
			REQUIRE(atom.is_group());

			const auto& group = atom.get_group();
			REQUIRE(group.get_type() == group::type::capturing);
			REQUIRE(!group.get_name().has_value());
		}

		SECTION("Non-capturing group") {
			std::u32string input = U"(?:abc)";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_group());

			const auto& group = atom.get_group();
			REQUIRE(group.get_type() == group::type::non_capturing);
		}

		SECTION("Named capturing group") {
			std::u32string input = U"(?<name>abc)";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_group());

			const auto& group = atom.get_group();
			REQUIRE(group.get_type() == group::type::capturing);
			REQUIRE(group.get_name().has_value());
			REQUIRE(group.get_name().value() == "name");
		}

		SECTION("Positive lookahead") {
			std::u32string input = U"(?=abc)";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_group());

			const auto& group = atom.get_group();
			REQUIRE(group.get_type() == group::type::positive_lookahead);
		}

		SECTION("Negative lookahead") {
			std::u32string input = U"(?!abc)";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_group());

			const auto& group = atom.get_group();
			REQUIRE(group.get_type() == group::type::negative_lookahead);
		}

		SECTION("Positive lookbehind") {
			std::u32string input = U"(?<=abc)";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_group());

			const auto& group = atom.get_group();
			REQUIRE(group.get_type() == group::type::positive_lookbehind);
		}

		SECTION("Negative lookbehind") {
			std::u32string input = U"(?<!abc)";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& atom = regex.get_disjunction().get_alternatives()[0].get_terms()[0].get_atom();
			REQUIRE(atom.is_group());

			const auto& group = atom.get_group();
			REQUIRE(group.get_type() == group::type::negative_lookbehind);
		}

		SECTION("Quantifiers on groups") {
			std::u32string input = U"(abc)+";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& terms = regex.get_disjunction().get_alternatives()[0].get_terms();
			REQUIRE(terms.size() == 1);

			const auto& atom = terms[0].get_atom();
			REQUIRE(atom.is_group());

			const auto& quantifier_opt = terms[0].get_quantifier();
			REQUIRE(quantifier_opt.has_value());
			REQUIRE(quantifier_opt.value().get_prefix() == quantifier::prefix::one_or_more);
		}
	}

	SECTION("Assertions") {
		SECTION("Line anchors") {
			std::u32string input = U"^abc$";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& disjunction = regex.get_disjunction();
			auto alternatives = disjunction.get_alternatives();
			REQUIRE(alternatives.size() == 1);

			const auto& terms = alternatives[0].get_terms();
			REQUIRE(terms.size() == 5);	 // ^, a, b, c, $

			// Start of line assertion
			REQUIRE(terms[0].is_assertion());
			const auto& start_assertion = terms[0].get_assertion();
			REQUIRE(start_assertion.get_type() == assertion::type::start_of_line);

			// Characters
			REQUIRE(!terms[1].is_assertion());
			const auto& atom_a = terms[1].get_atom();
			REQUIRE(atom_a.is_character());
			REQUIRE(atom_a.get_character() == U'a');

			REQUIRE(!terms[2].is_assertion());
			const auto& atom_b = terms[2].get_atom();
			REQUIRE(atom_b.is_character());
			REQUIRE(atom_b.get_character() == U'b');

			REQUIRE(!terms[3].is_assertion());
			const auto& atom_c = terms[3].get_atom();
			REQUIRE(atom_c.is_character());
			REQUIRE(atom_c.get_character() == U'c');

			// End of line assertion
			REQUIRE(terms[4].is_assertion());
			const auto& end_assertion = terms[4].get_assertion();
			REQUIRE(end_assertion.get_type() == assertion::type::end_of_line);
		}

		SECTION("Word boundary") {
			std::u32string input = U"\\b";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& terms = regex.get_disjunction().get_alternatives()[0].get_terms();
			REQUIRE(terms.size() == 1);
			REQUIRE(terms[0].is_assertion());

			const auto& assertion = terms[0].get_assertion();
			REQUIRE(assertion.get_type() == assertion::type::word_boundary);
		}

		SECTION("Non-word boundary") {
			std::u32string input = U"\\B";
			regular_expression regex;

			REQUIRE_NOTHROW(scan(input, regex));

			const auto& terms = regex.get_disjunction().get_alternatives()[0].get_terms();
			REQUIRE(terms.size() == 1);
			REQUIRE(terms[0].is_assertion());

			const auto& assertion = terms[0].get_assertion();
			REQUIRE(assertion.get_type() == assertion::type::non_word_boundary);
		}
	}

	SECTION("Error handling") {
		SECTION("Unterminated character class") {
			std::u32string input = U"[abc";
			regular_expression regex;

			REQUIRE_THROWS_AS(scan(input, regex),
							  tsccore::regex::unterminated_character_class);
		}

		SECTION("Invalid character class range") {
			std::u32string input = U"[z-a]";
			regular_expression regex;

			REQUIRE_THROWS_AS(scan(input, regex),
							  tsccore::regex::invalid_character_class_range);
		}

		SECTION("Unterminated group") {
			std::u32string input = U"(abc";
			regular_expression regex;

			REQUIRE_THROWS_AS(scan(input, regex),
							  tsccore::regex::unterminated_regular_expression_literal);
		}

		SECTION("Invalid hex escape") {
			std::u32string input = U"\\xZZ";
			regular_expression regex;

			REQUIRE_THROWS_AS(scan(input, regex),
							  tsccore::regex::invalid_escape_sequence);
		}

		SECTION("Invalid unicode escape") {
			std::u32string input = U"\\uZZZZ";
			regular_expression regex;

			REQUIRE_THROWS_AS(scan(input, regex),
							  tsccore::regex::invalid_escape_sequence);
		}

		SECTION("Orphaned quantifier") {
			std::u32string input = U"*abc";
			regular_expression regex;

			REQUIRE_THROWS_AS(scan(input, regex),
							  tsccore::regex::invalid_regular_expression);
		}

		SECTION("Unterminated quantifier") {
			std::u32string input = U"a{3";
			regular_expression regex;

			REQUIRE_THROWS_AS(scan(input, regex),
							  tsccore::regex::invalid_regular_expression);
		}
	}
}