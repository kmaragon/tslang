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

class regex_scanner_impl {
private:
	scanner& scanner_;
	char32_t current_char_;
	char32_t next_char_;
	size_t offset_;
	bool has_next_;

public:
	explicit regex_scanner_impl(scanner& scanner)
		: scanner_(scanner),
		  current_char_(0),
		  next_char_(0),
		  offset_(0),
		  has_next_(false) {
		advance();
		peek_next();
	}

	char32_t current() const { return current_char_; }

	char32_t peek() const { return has_next_ ? next_char_ : EOF_CHAR; }

	void advance() {
		if (has_next_) {
			current_char_ = next_char_;
			has_next_ = false;
		} else {
			current_char_ = scanner_.read_next();
		}
		++offset_;
	}

	void peek_next() {
		if (!has_next_) {
			next_char_ = scanner_.read_next();
			has_next_ = true;
		}
	}

	size_t get_offset() const { return offset_; }

	bool at_end() const { return current_char_ == EOF_CHAR; }
};

disjunction scan_disjunction(regex_scanner_impl& scanner);

group scan_group(regex_scanner_impl& scanner) {
	if (scanner.current() != U'(') {
		throw invalid_regular_expression(scanner.get_offset());
	}

	scanner.advance();	// skip '('

	group::type group_type = group::type::capturing;
	std::optional<std::string> name;

	// Check for special group types
	if (scanner.current() == U'?') {
		scanner.advance();

		switch (scanner.current()) {
			case U':':
				group_type = group::type::non_capturing;
				scanner.advance();
				break;
			case U'=':
				group_type = group::type::positive_lookahead;
				scanner.advance();
				break;
			case U'!':
				group_type = group::type::negative_lookahead;
				scanner.advance();
				break;
			case U'<':
				scanner.advance();
				if (scanner.current() == U'=') {
					group_type = group::type::positive_lookbehind;
					scanner.advance();
				} else if (scanner.current() == U'!') {
					group_type = group::type::negative_lookbehind;
					scanner.advance();
				} else {
					// Named capturing group ?<name>
					std::string group_name;
					while (scanner.current() != U'>' && !scanner.at_end()) {
						group_name += static_cast<char>(scanner.current());
						scanner.advance();
					}
					if (scanner.current() == U'>') {
						scanner.advance();
						name = group_name;
					}
				}
				break;
			default:
				throw invalid_regular_expression(scanner.get_offset());
		}
	}

	disjunction group_disjunction = scan_disjunction(scanner);

	if (scanner.current() != U')') {
		throw unterminated_regular_expression_literal(scanner.get_offset());
	}

	scanner.advance();	// skip ')'

	return group(group_type, std::move(group_disjunction), name);
}

char32_t scan_escape_sequence(regex_scanner_impl& scanner) {
	char32_t ch = scanner.current();
	scanner.advance();

	switch (ch) {
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
				if (!scanner.at_end() && std::isxdigit(scanner.current())) {
					char32_t digit = scanner.current();
					if (digit >= U'0' && digit <= U'9') {
						result = result * 16 + (digit - U'0');
					} else if (digit >= U'a' && digit <= U'f') {
						result = result * 16 + (digit - U'a' + 10);
					} else if (digit >= U'A' && digit <= U'F') {
						result = result * 16 + (digit - U'A' + 10);
					}
					scanner.advance();
				} else {
					throw invalid_escape_sequence(scanner.get_offset());
				}
			}
			return result;
		}

		case U'u': {
			// \uHHHH - unicode escape
			char32_t result = 0;
			for (int i = 0; i < 4; ++i) {
				if (!scanner.at_end() && std::isxdigit(scanner.current())) {
					char32_t digit = scanner.current();
					if (digit >= U'0' && digit <= U'9') {
						result = result * 16 + (digit - U'0');
					} else if (digit >= U'a' && digit <= U'f') {
						result = result * 16 + (digit - U'a' + 10);
					} else if (digit >= U'A' && digit <= U'F') {
						result = result * 16 + (digit - U'A' + 10);
					}
					scanner.advance();
				} else {
					throw invalid_escape_sequence(scanner.get_offset());
				}
			}
			return result;
		}

		default:
			// For other characters, treat as literal
			return ch;
	}
}

character_class scan_character_class(regex_scanner_impl& scanner) {
	if (scanner.current() != U'[') {
		throw invalid_regular_expression(scanner.get_offset());
	}

	scanner.advance();	// skip '['

	bool negated = false;
	if (scanner.current() == U'^') {
		negated = true;
		scanner.advance();
	}

	character_class char_class(negated);

	while (scanner.current() != U']' && !scanner.at_end()) {
		char32_t start_char = scanner.current();

		if (start_char == U'\\') {
			scanner.advance();
			start_char = scan_escape_sequence(scanner);
		} else {
			scanner.advance();
		}

		if (scanner.current() == U'-' && !scanner.at_end()) {
			scanner.advance();	// skip '-'

			if (scanner.current() == U']') {
				// '-' at end of character class, treat as literal
				char_class.add_character(start_char);
				char_class.add_character(U'-');
				break;
			}

			char32_t end_char = scanner.current();
			if (end_char == U'\\') {
				scanner.advance();
				end_char = scan_escape_sequence(scanner);
			} else {
				scanner.advance();
			}

			if (start_char > end_char) {
				throw invalid_character_class_range(scanner.get_offset());
			}

			char_class.add_range(start_char, end_char);
		} else {
			char_class.add_character(start_char);
		}
	}

	if (scanner.current() != U']') {
		throw unterminated_character_class(scanner.get_offset());
	}

	scanner.advance();	// skip ']'
	return char_class;
}

atom scan_atom(regex_scanner_impl& scanner) {
	char32_t ch = scanner.current();

	switch (ch) {
		case U'.':
			scanner.advance();
			return atom(atom::builtin_class::dot);

		case U'[':
			return atom(scan_character_class(scanner));

		case U'(':
			return atom(scan_group(scanner));

		case U'\\': {
			scanner.advance();	// skip backslash
			char32_t escaped = scanner.current();

			switch (escaped) {
				case U'w':
					scanner.advance();
					return atom(atom::builtin_class::word);
				case U'W':
					scanner.advance();
					return atom(atom::builtin_class::non_word);
				case U'd':
					scanner.advance();
					return atom(atom::builtin_class::digit);
				case U'D':
					scanner.advance();
					return atom(atom::builtin_class::non_digit);
				case U's':
					scanner.advance();
					return atom(atom::builtin_class::whitespace);
				case U'S':
					scanner.advance();
					return atom(atom::builtin_class::non_whitespace);
				case U'b':
				case U'B':
					// These should be handled as assertions, not atoms
					// Put the backslash back and throw an error since this
					// shouldn't happen
					throw invalid_regular_expression(scanner.get_offset());
				default:
					return atom(scan_escape_sequence(scanner));
			}
		}

		default:
			if (ch == U'*' || ch == U'+' || ch == U'?' || ch == U'{' ||
				ch == U'}' || ch == U'|' || ch == U')' || ch == EOF_CHAR) {
				throw invalid_regular_expression(scanner.get_offset());
			}
			scanner.advance();
			return atom(ch);
	}
}

assertion scan_assertion(regex_scanner_impl& scanner) {
	char32_t ch = scanner.current();
	scanner.advance();

	switch (ch) {
		case U'^':
			return assertion(assertion::type::start_of_line);
		case U'$':
			return assertion(assertion::type::end_of_line);
		case U'b':
			return assertion(assertion::type::word_boundary);
		case U'B':
			return assertion(assertion::type::non_word_boundary);
		default:
			throw invalid_regular_expression(scanner.get_offset());
	}
}

std::optional<quantifier> scan_quantifier(regex_scanner_impl& scanner) {
	char32_t ch = scanner.current();

	switch (ch) {
		case U'*':
			scanner.advance();
			return quantifier{quantifier::prefix::zero_or_more};
		case U'+':
			scanner.advance();
			return quantifier{quantifier::prefix::one_or_more};
		case U'?':
			scanner.advance();
			return quantifier{quantifier::prefix::zero_or_one};
		case U'{': {
			scanner.advance();	// skip '{'

			// Parse min value
			size_t min = 0;
			while (std::isdigit(scanner.current()) && !scanner.at_end()) {
				min = min * 10 + (scanner.current() - U'0');
				scanner.advance();
			}

			size_t max = min;

			if (scanner.current() == U',') {
				scanner.advance();	// skip ','

				if (scanner.current() == U'}') {
					// {min,} - unlimited max
					max = SIZE_MAX;
				} else {
					// {min,max}
					max = 0;
					while (std::isdigit(scanner.current()) &&
						   !scanner.at_end()) {
						max = max * 10 + (scanner.current() - U'0');
						scanner.advance();
					}
				}
			}

			if (scanner.current() != U'}') {
				throw invalid_regular_expression(scanner.get_offset());
			}

			scanner.advance();	// skip '}'

			return quantifier(std::make_pair(min, max));
		}
		default:
			return std::nullopt;
	}
}

term scan_term(regex_scanner_impl& scanner) {
	switch (scanner.current()) {
		case U'^':
		case U'$': {
			assertion assert = scan_assertion(scanner);
			return term(assert);
		}
		case U'\\': {
			// Look ahead to see if it's a boundary assertion
			scanner.peek_next();
			if (scanner.peek() == U'b' || scanner.peek() == U'B') {
				scanner.advance();	// skip backslash
				assertion assert = scan_assertion(scanner);
				return term(assert);
			} else {
				// Let scan_atom handle the backslash
				atom a = scan_atom(scanner);
				auto quant = scan_quantifier(scanner);
				return term(a, quant);
			}
		}
		default: {
			atom a = scan_atom(scanner);
			auto quant = scan_quantifier(scanner);
			return term(a, quant);
		}
	}
}

alternative scan_alternative(regex_scanner_impl& scanner) {
	std::vector<term> terms;

	while (!scanner.at_end() && scanner.current() != U'|' &&
		   scanner.current() != U')') {
		terms.push_back(scan_term(scanner));
	}

	return alternative(std::move(terms));
}

disjunction scan_disjunction(regex_scanner_impl& scanner) {
	disjunction result;

	result.add_alternative(scan_alternative(scanner));

	while (scanner.current() == U'|') {
		scanner.advance();	// skip '|'
		result.add_alternative(scan_alternative(scanner));
	}

	return result;
}

}  // namespace

void tsccore::regex::scan(scanner& scanner, regular_expression& into) {
	regex_scanner_impl impl(scanner);

	try {
		disjunction main_disjunction = scan_disjunction(impl);
		into.set_disjunction(std::move(main_disjunction));
	} catch (const tsccore::regex::regex_error&) {
		throw;
	} catch (const std::exception&) {
		throw invalid_regular_expression(impl.get_offset());
	}
}
