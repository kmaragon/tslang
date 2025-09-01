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

#include "scan_regex.hpp"
#include <cctype>
#include <limits>
#include <stdexcept>
#include "alternative.hpp"
#include "assertion.hpp"
#include "atom.hpp"
#include "character_class.hpp"
#include "disjunction.hpp"
#include "error.hpp"
#include "group.hpp"
#include "quantifier.hpp"

using namespace tsccore::regex;

namespace {
constexpr char32_t EOF_CHAR = static_cast<char32_t>(-1);

constexpr char32_t current_char(const std::u32string_view& input,
								const size_t pos) noexcept {
	return pos < input.length() ? input[pos] : EOF_CHAR;
}

constexpr char32_t peek_char(const std::u32string_view& input,
							 const size_t pos) noexcept {
	return pos + 1 < input.length() ? input[pos + 1] : EOF_CHAR;
}

constexpr bool at_end(const std::u32string_view& input, size_t pos) noexcept {
	return pos >= input.length();
}

disjunction scan_disjunction(const std::u32string_view& input, size_t& pos);

group scan_group(const std::u32string_view& input, size_t& pos) {
	if (current_char(input, pos) != U'(') {
		throw invalid_regular_expression(pos);
	}

	++pos;	// skip '('

	group::type group_type = group::type::capturing;
	std::optional<std::u32string> name;

	// Check for special group types
	if (current_char(input, pos) == U'?') {
		++pos;

		switch (current_char(input, pos)) {
			case U':':
				group_type = group::type::non_capturing;
				++pos;
				break;
			case U'=':
				group_type = group::type::positive_lookahead;
				++pos;
				break;
			case U'!':
				group_type = group::type::negative_lookahead;
				++pos;
				break;
			case U'<':
				++pos;
				if (current_char(input, pos) == U'=') {
					group_type = group::type::positive_lookbehind;
					++pos;
				} else if (current_char(input, pos) == U'!') {
					group_type = group::type::negative_lookbehind;
					++pos;
				} else {
					// Named capturing group ?<name>
					std::u32string group_name;
					group_name.reserve(8);
					while (current_char(input, pos) != U'>' &&
						   !at_end(input, pos)) {
						group_name += current_char(input, pos);
						++pos;
					}
					if (current_char(input, pos) == U'>') {
						++pos;
						name = std::move(group_name);
					}
				}
				break;
			default:
				throw invalid_regular_expression(pos);
		}
	}

	disjunction group_disjunction = scan_disjunction(input, pos);

	if (current_char(input, pos++) != U')') {
		throw unterminated_regular_expression_literal(pos);
	}

	return {group_type, std::move(group_disjunction), name};
}

char32_t scan_escape_sequence(const std::u32string_view& input, size_t& pos) {
	switch (char32_t ch = current_char(input, pos++)) {
		case U'n':
			return U'\n';
		case U'r':
			return U'\r';
		case U't':
			return U'\t';
		case U'f':
			return U'\f';
		case U'v':
			return U'\v';
		case U'0':
			return U'\0';
		case U'\\':
			return U'\\';
		case U'/':
			return U'/';
		case U'^':
			return U'^';
		case U'$':
			return U'$';
		case U'.':
			return U'.';
		case U'*':
			return U'*';
		case U'+':
			return U'+';
		case U'?':
			return U'?';
		case U'(':
			return U'(';
		case U')':
			return U')';
		case U'[':
			return U'[';
		case U']':
			return U']';
		case U'{':
			return U'{';
		case U'}':
			return U'}';
		case U'|':
			return U'|';

		case U'x': {
			// \xHH - hexadecimal escape
			char32_t result = 0;
			for (int i = 0; i < 2; ++i) {
				if (!at_end(input, pos) &&
					std::iswxdigit(current_char(input, pos))) {
					char32_t digit = current_char(input, pos);
					if (digit >= U'0' && digit <= U'9') {
						result = result * 16 + (digit - U'0');
					} else if (digit >= U'a' && digit <= U'f') {
						result = result * 16 + (digit - U'a' + 10);
					} else if (digit >= U'A' && digit <= U'F') {
						result = result * 16 + (digit - U'A' + 10);
					}
					++pos;
				} else {
					throw invalid_escape_sequence(pos);
				}
			}
			return result;
		}

		case U'u': {
			// \uHHHH - unicode escape
			char32_t result = 0;
			for (int i = 0; i < 4; ++i) {
				if (!at_end(input, pos) &&
					std::iswxdigit(current_char(input, pos))) {
					char32_t digit = current_char(input, pos);
					if (digit >= U'0' && digit <= U'9') {
						result = result * 16 + (digit - U'0');
					} else if (digit >= U'a' && digit <= U'f') {
						result = result * 16 + (digit - U'a' + 10);
					} else if (digit >= U'A' && digit <= U'F') {
						result = result * 16 + (digit - U'A' + 10);
					}
					++pos;
				} else {
					throw invalid_escape_sequence(pos);
				}
			}
			return result;
		}

		default:
			// For other characters, treat as literal
			return ch;
	}
}

character_class scan_character_class(const std::u32string_view& input,
									 size_t& pos) {
	if (current_char(input, pos) != U'[') {
		throw invalid_regular_expression(pos);
	}

	++pos;	// skip '['

	bool negated = false;
	if (current_char(input, pos) == U'^') {
		negated = true;
		++pos;
	}

	character_class char_class(negated);

	while (current_char(input, pos) != U']' && !at_end(input, pos)) {
		char32_t start_char = current_char(input, pos);

		if (start_char == U'\\') {
			++pos;
			start_char = scan_escape_sequence(input, pos);
		} else {
			++pos;
		}

		if (current_char(input, pos) == U'-' && !at_end(input, pos)) {
			++pos;	// skip '-'

			if (current_char(input, pos) == U']') {
				// '-' at end of character class, treat as literal
				char_class.add_character(start_char);
				char_class.add_character(U'-');
				break;
			}

			char32_t end_char = current_char(input, pos);
			if (end_char == U'\\') {
				++pos;
				end_char = scan_escape_sequence(input, pos);
			} else {
				++pos;
			}

			if (start_char > end_char) {
				throw invalid_character_class_range(pos);
			}

			char_class.add_range(start_char, end_char);
		} else {
			char_class.add_character(start_char);
		}
	}

	if (current_char(input, pos) != U']') {
		throw unterminated_character_class(pos);
	}

	++pos;	// skip ']'
	return char_class;
}

atom scan_atom(const std::u32string_view& input, size_t& pos) {
	switch (const char32_t ch = current_char(input, pos)) {
		case U'.':
			++pos;
			return atom::builtin_class::dot;

		case U'[':
			return scan_character_class(input, pos);

		case U'(':
			return scan_group(input, pos);

		case U'\\': {
			// skip backslash
			switch (current_char(input, ++pos)) {
				case U'w':
					++pos;
					return atom::builtin_class::word;
				case U'W':
					++pos;
					return atom::builtin_class::non_word;
				case U'd':
					++pos;
					return atom::builtin_class::digit;
				case U'D':
					++pos;
					return atom::builtin_class::non_digit;
				case U's':
					++pos;
					return atom::builtin_class::whitespace;
				case U'S':
					++pos;
					return atom::builtin_class::non_whitespace;
				case U'b':
				case U'B':
					// These should be handled as assertions, not atoms
					// Put the backslash back and throw an error since this
					// shouldn't happen
					throw invalid_regular_expression(pos);
				default:
					return scan_escape_sequence(input, pos);
			}
		}

		default:
			if (ch == U'*' || ch == U'+' || ch == U'?' || ch == U'{' ||
				ch == U'}' || ch == U'|' || ch == U')' || ch == EOF_CHAR) {
				throw invalid_regular_expression(pos);
			}
			++pos;
			return ch;
	}
}

assertion scan_assertion(const std::u32string_view& input, size_t& pos) {
	switch (current_char(input, pos++)) {
		case U'^':
			return assertion(assertion::type::start_of_line);
		case U'$':
			return assertion(assertion::type::end_of_line);
		case U'b':
			return assertion(assertion::type::word_boundary);
		case U'B':
			return assertion(assertion::type::non_word_boundary);
		default:
			throw invalid_regular_expression(pos);
	}
}

std::optional<quantifier> scan_quantifier(const std::u32string_view& input,
										  size_t& pos) {
	switch (current_char(input, pos)) {
		case U'*':
			++pos;
			return quantifier{quantifier::prefix::zero_or_more};
		case U'+':
			++pos;
			return quantifier{quantifier::prefix::one_or_more};
		case U'?':
			++pos;
			return quantifier{quantifier::prefix::zero_or_one};
		case U'{': {
			++pos;	// skip '{'

			// Parse min value
			size_t min = 0;
			while (std::iswdigit(current_char(input, pos)) &&
				   !at_end(input, pos)) {
				min = min * 10 + (current_char(input, pos) - U'0');
				++pos;
			}

			size_t max = min;

			if (current_char(input, pos) == U',') {
				++pos;	// skip ','

				if (current_char(input, pos) == U'}') {
					// {min,} - unlimited max
					max = std::numeric_limits<std::size_t>::max();
				} else {
					// {min,max}
					max = 0;
					while (std::iswdigit(current_char(input, pos)) &&
						   !at_end(input, pos)) {
						max = max * 10 + (current_char(input, pos) - U'0');
						++pos;
					}
				}
			}

			if (current_char(input, pos) != U'}') {
				throw invalid_regular_expression(pos);
			}

			++pos;	// skip '}'

			return quantifier(std::make_pair(min, max));
		}
		default:
			return std::nullopt;
	}
}

term scan_term(const std::u32string_view& input, size_t& pos) {
	switch (current_char(input, pos)) {
		case U'^':
		case U'$': {
			assertion assert = scan_assertion(input, pos);
			return assert;
		}
		case U'\\': {
			// Look ahead to see if it's a boundary assertion
			if (peek_char(input, pos) == U'b' ||
				peek_char(input, pos) == U'B') {
				++pos;	// skip backslash
				assertion assert = scan_assertion(input, pos);
				return assert;
			}

			// Let scan_atom handle the backslash
			atom a = scan_atom(input, pos);
			auto quant = scan_quantifier(input, pos);
			return {a, quant};
		}
		default: {
			atom a = scan_atom(input, pos);
			auto quant = scan_quantifier(input, pos);
			return {a, quant};
		}
	}
}

alternative scan_alternative(const std::u32string_view& input, size_t& pos) {
	std::vector<term> terms;

	while (!at_end(input, pos) && current_char(input, pos) != U'|' &&
		   current_char(input, pos) != U')') {
		terms.push_back(scan_term(input, pos));
	}

	return alternative(std::move(terms));
}

disjunction scan_disjunction(const std::u32string_view& input, size_t& pos) {
	disjunction result;

	result.add_alternative(scan_alternative(input, pos));

	while (current_char(input, pos) == U'|') {
		++pos;	// skip '|'
		result.add_alternative(scan_alternative(input, pos));
	}

	return result;
}

}  // namespace

void tsccore::regex::scan(const std::u32string_view& input,
						  regular_expression& into) {
	size_t pos = 0;

	try {
		disjunction main_disjunction = scan_disjunction(input, pos);
		into.set_disjunction(std::move(main_disjunction));
	} catch (const regex_error&) {
		throw;
	} catch (const std::exception&) {
		throw invalid_regular_expression(pos);
	}
}
