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
#include <sstream>
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

class string_scanner : public scanner {
private:
	std::u32string input_;
	size_t position_;

public:
	explicit string_scanner(const std::u32string& input)
		: input_(input), position_(0) {}

	char32_t read_next() override {
		if (position_ >= input_.length()) {
			return static_cast<char32_t>(-1);  // EOF
		}
		return input_[position_++];
	}
};

TEST_CASE("Regular Expression Scanning", "[regex]") {
	SECTION("Simple character") {
		std::u32string input = U"a";
		string_scanner scanner(input);
		regular_expression regex;

		REQUIRE_NOTHROW(scan(scanner, regex));

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

	SECTION("Character class") {
		std::u32string input = U"[abc]";
		string_scanner scanner(input);
		regular_expression regex;

		REQUIRE_NOTHROW(scan(scanner, regex));

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
		string_scanner scanner(input);
		regular_expression regex;

		REQUIRE_NOTHROW(scan(scanner, regex));

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
		string_scanner scanner(input);
		regular_expression regex;

		REQUIRE_NOTHROW(scan(scanner, regex));

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

	SECTION("Quantifiers") {
		std::u32string input = U"a*";
		string_scanner scanner(input);
		regular_expression regex;

		REQUIRE_NOTHROW(scan(scanner, regex));

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

	SECTION("Disjunction") {
		std::u32string input = U"a|b";
		string_scanner scanner(input);
		regular_expression regex;

		REQUIRE_NOTHROW(scan(scanner, regex));

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

	SECTION("Builtin character classes") {
		std::u32string input = U"\\d";
		string_scanner scanner(input);
		regular_expression regex;

		REQUIRE_NOTHROW(scan(scanner, regex));

		const auto& disjunction = regex.get_disjunction();
		auto alternatives = disjunction.get_alternatives();
		REQUIRE(alternatives.size() == 1);

		const auto& terms = alternatives[0].get_terms();
		REQUIRE(terms.size() == 1);

		const auto& atom = terms[0].get_atom();
		REQUIRE(atom.is_builtin_class());
		REQUIRE(atom.get_builtin_class() == atom::builtin_class::digit);
	}

	SECTION("Assertions") {
		std::u32string input = U"^abc$";
		string_scanner scanner(input);
		regular_expression regex;

		REQUIRE_NOTHROW(scan(scanner, regex));

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

	SECTION("Error handling") {
		SECTION("Unterminated character class") {
			std::u32string input = U"[abc";
			string_scanner scanner(input);
			regular_expression regex;

			REQUIRE_THROWS_AS(scan(scanner, regex),
							  tsccore::regex::unterminated_character_class);
		}

		SECTION("Invalid character class range") {
			std::u32string input = U"[z-a]";
			string_scanner scanner(input);
			regular_expression regex;

			REQUIRE_THROWS_AS(scan(scanner, regex),
							  tsccore::regex::invalid_character_class_range);
		}
	}
}