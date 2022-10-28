/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2022. Keef Aragon
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

#include "lexer.hpp"
#include <cstring>
#include "error/expected_command.hpp"
#include "error/invalid_character.hpp"
#include "error/misplaced_shebang.hpp"
#include "error/unexpected_token.hpp"
#include "token.hpp"
#include "tokens/shebang_token.hpp"

using namespace tscc::lex;

lex_error::lex_error(const tscc::lex::source_location& location) noexcept
	: location_(location) {}

const source_location& lex_error::location() const noexcept {
	return location_;
}

lexer::iterator::value_type& lexer::iterator::operator*() const {
	return *token_;
}

lexer::iterator::pointer_type lexer::iterator::operator->() const {
	return token_.get();
}

lexer::iterator& lexer::iterator::operator++() {
	if (token_.use_count() > 1)
		token_ = std::make_shared<token>();

	if (!lexer_->scan(*token_))
		token_->undefine();
	return *this;
}

lexer::iterator lexer::iterator::operator++(int) {
	auto cp = *this;
	++(*this);
	return cp;
}

bool lexer::iterator::operator==(const iterator& other) const {
	return (token_ == other.token_) || (*token_ == *other.token_);
}

bool lexer::iterator::operator!=(const iterator& other) const {
	return !operator==(other);
}

lexer::iterator::iterator(lexer* source)
	: token_(std::make_shared<token>()), lexer_(source) {}

lexer::lexer(std::istream& stream, std::shared_ptr<source> stream_metadata)
	: stream_(stream),
	  source_(std::move(stream_metadata)),
	  buffer_offset_(0),
	  end_(this),
	  cr_(false) {
	wbuffer_.reserve(buffer_size);
}

lexer::iterator lexer::begin() {
	lexer::iterator result(this);
	++result;
	return result;
}

lexer::iterator lexer::end() {
	return end_;
}

inline std::size_t lexer::next_code_point(wchar_t& into,
										  std::size_t look_forward) {
	auto read_more = [this](std::size_t needed) -> std::size_t {
		if (stream_.eof())
			return 0;

		auto sn = needed;
		while (true) {
			auto preserve_size = rbuffer_.size() - buffer_offset_;
			memmove(rbuffer_.data(), rbuffer_.data() + buffer_offset_,
					preserve_size);
			buffer_offset_ = 0;

			rbuffer_.resize(buffer_size);
			auto read = stream_.readsome(
				rbuffer_.data() + preserve_size,
				static_cast<std::streamsize>(buffer_size - preserve_size));

			if (read <= 0) {
				rbuffer_.resize(preserve_size);
				return sn - needed;
			}

			rbuffer_.resize(buffer_offset_ + read);
			needed -= std::min(needed, static_cast<std::size_t>(read));

			if (!needed)
				return sn;
		}
	};

	if ((buffer_offset_ + look_forward) >= rbuffer_.size()) {
		if (!read_more(1))
			return 0;
	}

	// get one character
	auto chr0 =
		static_cast<unsigned char>(rbuffer_[buffer_offset_ + look_forward]);
	// if the value is either below 0x7f or isn't a valid utf-8 codepoint
	if (chr0 <= 0x7f || (chr0 & 0xc0) != 0xc0) {
		into = chr0;
		return 1;
	}

	if ((chr0 & 0xe0) == 0xc0) {
		// 2 characters -> 11 bits
		if ((buffer_offset_ + look_forward + 1) >= rbuffer_.size() &&
			read_more(1) < 1) {
			into = chr0;
			return 1;
		}

		auto chr1 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 1]);
		if ((chr1 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		into = (static_cast<wchar_t>(chr0 & 0x1f) << 6) |
			   static_cast<wchar_t>(chr1 & 0x3f);
		return 2;
	}

	if ((chr0 & 0xf0) == 0xe0) {
		// 3 characters -> 16 bits
		if ((buffer_offset_ + look_forward + 2) >= rbuffer_.size() &&
			read_more(2) < 2) {
			into = chr0;
			return 1;
		}

		auto chr1 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 1]);
		if ((chr1 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		auto chr2 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 2]);
		if ((chr2 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		into = (static_cast<wchar_t>(chr0 & 0x0f) << 12) |
			   (static_cast<wchar_t>(chr1 & 0x3f) << 6) |
			   static_cast<wchar_t>(chr2 & 0x3f);
		return 3;
	}

	if ((chr0 & 0xf8) == 0xf0) {
		// 4 characters -> 21 bits
		if ((buffer_offset_ + look_forward + 3) >= rbuffer_.size() &&
			read_more(3) < 3) {
			into = chr0;
			return 1;
		}

		auto chr1 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 1]);
		if ((chr1 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		auto chr2 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 2]);
		if ((chr2 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		auto chr3 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 3]);
		if ((chr3 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		into = (static_cast<wchar_t>(chr0 & 0x07) << 18) |
			   (static_cast<wchar_t>(chr1 & 0x3f) << 12) |
			   (static_cast<wchar_t>(chr2 & 0x3f) << 6) |
			   static_cast<wchar_t>(chr3 & 0x3f);
		return 4;
	}

	if ((chr0 & 0xfc) == 0xf8) {
		// 5 characters -> 26 bits
		if ((buffer_offset_ + look_forward + 4) >= rbuffer_.size() &&
			read_more(4) < 4) {
			into = chr0;
			return 1;
		}

		auto chr1 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 1]);
		if ((chr1 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		auto chr2 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 2]);
		if ((chr2 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		auto chr3 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 3]);
		if ((chr3 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		auto chr4 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 4]);
		if ((chr4 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		into = (static_cast<wchar_t>(chr0 & 0x03) << 24) |
			   (static_cast<wchar_t>(chr1 & 0x3f) << 18) |
			   (static_cast<wchar_t>(chr2 & 0x3f) << 12) |
			   (static_cast<wchar_t>(chr3 & 0x3f) << 6) |
			   static_cast<wchar_t>(chr4 & 0x3f);
		return 5;
	}

	if ((chr0 & 0xfe) == 0xfc) {
		// 6 characters -> 31 bits
		if ((buffer_offset_ + look_forward + 5) >= rbuffer_.size() &&
			read_more(5) < 5) {
			into = chr0;
			return 1;
		}

		auto chr1 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 1]);
		if ((chr1 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		auto chr2 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 2]);
		if ((chr2 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		auto chr3 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 3]);
		if ((chr3 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		auto chr4 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 4]);
		if ((chr4 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		auto chr5 = static_cast<unsigned char>(
			rbuffer_[buffer_offset_ + look_forward + 5]);
		if ((chr5 & 0xc0) != 0x80) {
			into = chr0;
			return 1;
		}

		into = (static_cast<wchar_t>(chr0 & 0x01) << 30) |
			   (static_cast<wchar_t>(chr1 & 0x3f) << 24) |
			   (static_cast<wchar_t>(chr2 & 0x3f) << 18) |
			   (static_cast<wchar_t>(chr3 & 0x3f) << 12) |
			   (static_cast<wchar_t>(chr4 & 0x3f) << 6) |
			   static_cast<wchar_t>(chr5 & 0x3f);
		return 6;
	}

	return chr0;
}

void lexer::scan_shebang(std::size_t shebang_offset, tscc::lex::token& into) {
	auto shebang_location = location();
	advance(shebang_offset);

	scan_line_into_wbuffer();

	if (wbuffer_.empty())
		throw expected_command(shebang_location);

	into.emplace_token<tokens::shebang_token>(shebang_location,
											  std::move(wbuffer_));
}

void lexer::scan_string(tscc::lex::token& into) {
	throw std::system_error(std::make_error_code(std::errc::not_supported));
}

void lexer::scan_string_template(tscc::lex::token& into) {
	throw std::system_error(std::make_error_code(std::errc::not_supported));
}

void lexer::scan_line_comment(std::size_t comment_offset, tscc::lex::token& into) {
	auto comment_location = location();
	advance(comment_offset);

	scan_line_into_wbuffer();

	into.emplace_token<tokens::comment_token>(comment_location,
											  std::move(wbuffer_));
}

void lexer::scan_multiline_comment(tscc::lex::token& into, bool is_jsdoc) {
	throw std::system_error(std::make_error_code(std::errc::not_supported));
}

void lexer::scan_binary_number(tscc::lex::token& into) {
	throw std::system_error(std::make_error_code(std::errc::not_supported));
}

bool lexer::scan_octal_number(tscc::lex::token& into, bool throw_on_invalid) {
	throw std::system_error(std::make_error_code(std::errc::not_supported));
}

void lexer::scan_decimal_number(tscc::lex::token& into) {
	throw std::system_error(std::make_error_code(std::errc::not_supported));
}

void lexer::scan_hex_number(tscc::lex::token& into) {
	throw std::system_error(std::make_error_code(std::errc::not_supported));
}

void lexer::scan_conflict_marker(tscc::lex::token& into) {
	throw std::system_error(std::make_error_code(std::errc::not_supported));
}

bool lexer::scan_jsx_token(tscc::lex::token& into) {
	throw std::system_error(std::make_error_code(std::errc::not_supported));
}

void lexer::scan_unicode_escape(tscc::lex::token& into,
								std::size_t min_size,
								bool scan_as_many_as_possible,
								bool can_have_separators) {
	throw std::system_error(std::make_error_code(std::errc::not_supported));
}

bool lexer::try_scan_identifier(tscc::lex::token& into, bool is_private) {
	throw std::system_error(std::make_error_code(std::errc::not_supported));
}

constexpr bool lexer::is_decimal_digit(wchar_t ch) {
	return (ch >= L'0' && ch <= L'9') || (ch >= 0xff10 && ch <= 0xff19);
}

constexpr bool lexer::is_octal_digit(wchar_t ch) {
	return (ch >= L'0' && ch <= L'8') || (ch >= 0xff10 && ch <= 0xff18);
}

constexpr bool lexer::is_hex_digit(wchar_t ch) {
	return (ch >= L'0' && ch <= L'8') || (ch >= L'A' && ch <= L'F') ||
		   (ch >= L'a' && ch <= L'f') || (ch >= 0xff21 && ch <= 0xff26) ||
		   (ch >= 0xff41 && ch <= 0xff46) || (ch >= 0xff10 && ch <= 0xff18);
}

constexpr bool lexer::is_alpha(wchar_t ch) {
	return (ch >= L'A' && ch <= L'Z') || (ch >= L'a' && ch <= L'z') ||
		   (ch >= 0xff21 && ch <= 0xff3a) || (ch >= 0xff41 && ch <= 0xff5a);
}

void lexer::scan_line_into_wbuffer(bool trim) {
	wchar_t first{};

	// skip any whitespace
	while (true) {
		auto nc = next_code_point(first);
		if (!nc) {
			wbuffer_.clear();
			return;
		}

		advance(nc);
		if (!trim || !std::iswspace(first))
			break;
	}

	wbuffer_.reserve(buffer_size);
	wbuffer_.assign(&first, 1);

	// read the command until a eof or an eol
	while (true) {
		auto nc = next_code_point(first);
		if (!nc) {
			if (trim) {
				// trim any whitespace off of the end
				std::size_t end = wbuffer_.size();
				while (std::iswspace(wbuffer_[end - 1])) {
					end--;
				}

				wbuffer_.erase(end);
			}
			return;
		}

		advance(nc);
		if (first == '\n') {
			if (trim) {
				// trim any whitespace off of the end
				std::size_t end = wbuffer_.size();
				while (std::iswspace(wbuffer_[end - 1])) {
					end--;
				}

				wbuffer_.erase(end);
			}
			gpos_.advance_line();
			return;
		}

		if (wbuffer_.capacity() == wbuffer_.size()) {
			wbuffer_.reserve(wbuffer_.size() + buffer_size);
		}
		wbuffer_.push_back(first);
	}
}

bool lexer::scan(tscc::lex::token& into) {
	while (true) {
		wchar_t ch{};
		auto pos = next_code_point(ch);

		if (!ch)
			return false;

		// check for shebang
		if (gpos_.offset == 0 && ch == L'#') {
			wchar_t next{};
			auto gs = next_code_point(next, pos);
			if ((gs > 0) && next == L'!') {
				scan_shebang(pos + gs, into);
				return true;
			}
		}

		switch (ch) {
			case L'\r': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if ((gs > 0) && next == L'\n') {
					pos += gs;
				}

				advance(pos);
				gpos_.advance_line();
				into.emplace_token<tokens::newline_token>(loc);
				return true;
			}
			case L'\n': {
				auto loc = location();
				advance(pos);
				gpos_.advance_line();
				into.emplace_token<tokens::newline_token>(loc);
				return true;
			}
			case L'\t':	  // tab
			case 0x0b:	  // vertical tab
			case 0x0c:	  // form feed
			case L' ':	  // space
			case 0xa0:	  // nbsp
			case 0x1680:  // ogham space
			case 0x2000:  // en quad
			case 0x2001:  // em quad
			case 0x2002:  // en space
			case 0x2003:  // em space
			case 0x2004:  // three per em space
			case 0x2005:  // four per em space
			case 0x2006:  // six per em space
			case 0x2007:  // figure space
			case 0x2008:  // punctuation space
			case 0x2009:  // thin space
			case 0x200a:  // hair space
			case 0x200b:  // zero-width space
			case 0x202f:  // narrow no break space
			case 0x205f:  // medium mathematical space
			case 0x3000:  // ideographic space
			case 0xfeff:  // zero-width nbsp / bom
				advance(pos);
				break;
			case L'!': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == L'=') {
						wchar_t excnext{};
						auto ggs = next_code_point(excnext, pos + gs);
						if (ggs > 0 && excnext == '=') {
							advance(pos + gs + ggs);
							into.emplace_token<tokens::exclamation_eq_eq_token>(
								loc);
							return true;
						}

						advance(pos + gs);
						into.emplace_token<tokens::exclamation_eq_token>(loc);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::exclamation_token>(loc);
				return true;
			}
			case L'"':
			case L'\'':
				scan_string(into);
				return true;
			case L'`':
				scan_string_template(into);
				return true;
			case L'%': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if ((gs > 0) && next == L'=') {
					advance(pos + gs);
					into.emplace_token<tokens::percent_eq_token>(loc);
					return true;
				}

				advance(pos);
				into.emplace_token<tokens::percent_token>(loc);
				return true;
			}
			case L'&': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == L'=') {
						advance(pos + gs);
						into.emplace_token<tokens::ampersand_eq_token>(loc);
						return true;
					}

					if (next == L'&') {
						wchar_t ampnext{};
						auto ggs = next_code_point(ampnext, pos + gs);
						if (ggs > 0 && ampnext == '=') {
							advance(pos + gs + ggs);
							into.emplace_token<
								tokens::double_ampersand_eq_token>(loc);
							return true;
						}

						advance(pos + gs);
						into.emplace_token<tokens::double_ampersand_token>(loc);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::ampersand_token>(loc);
				return true;
			}
			case L'(':
				into.emplace_token<tokens::open_paren_token>(location());
				advance(pos);
				break;
			case L')':
				into.emplace_token<tokens::close_paren_token>(location());
				advance(pos);
				break;
			case L'*': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == L'=') {
						advance(pos + gs);
						into.emplace_token<tokens::asterisk_eq_token>(loc);
						return true;
					}

					if (next == L'*') {
						wchar_t astnext{};
						auto ggs = next_code_point(astnext, pos + gs);
						if (ggs > 0 && astnext == '=') {
							advance(pos + gs + ggs);
							into.emplace_token<
								tokens::double_asterisk_eq_token>(loc);
							return true;
						}

						advance(pos + gs);
						into.emplace_token<tokens::double_asterisk_token>(loc);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::asterisk_token>(loc);
				return true;
			}
			case L'+': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == L'+') {
						advance(pos + gs);
						into.emplace_token<tokens::double_plus_token>(loc);
						return true;
					}

					if (next == L'=') {
						advance(pos + gs);
						into.emplace_token<tokens::plus_eq_token>(loc);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::plus_token>(loc);
				return true;
			}
			case L',':
				into.emplace_token<tokens::comma_token>(location());
				advance(pos);
				break;
			case L'-': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == L'-') {
						advance(pos + gs);
						into.emplace_token<tokens::double_minus_token>(loc);
						return true;
					}

					if (next == L'=') {
						advance(pos + gs);
						into.emplace_token<tokens::minus_eq_token>(loc);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::minus_token>(loc);
				return true;
			}
			case L'.': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == L'.') {
						wchar_t dotnext{};
						auto ggs = next_code_point(dotnext, pos + gs);
						if (ggs > 0 && dotnext == '.') {
							advance(pos + gs + ggs);
							into.emplace_token<tokens::triple_dot_token>(loc);
							return true;
						}
					} else if (is_decimal_digit(next)) {
						scan_decimal_number(into);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::dot_token>(loc);
				return true;
			}
			case L'/': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == L'=') {
						advance(pos + gs);
						into.emplace_token<tokens::slash_eq_token>(loc);
						return true;
					}

					if (next == L'/') {
						scan_line_comment(pos + gs, into);
						return true;
					}

					if (next == L'*') {
						bool is_jsdoc = false;

						wchar_t astnext{};
						auto ggs = next_code_point(astnext, pos + gs);
						if (ggs > 0 && astnext == '*') {
							gs += ggs;
							is_jsdoc = true;
						}

						scan_multiline_comment(into, is_jsdoc);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::slash_token>(loc);
				return true;
			}
			case L'0': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == 'x' || next == 'X') {
						advance(pos + gs);
						scan_hex_number(into);
						return true;
					}

					if (next == 'b' || next == 'B') {
						advance(pos + gs);
						scan_binary_number(into);
						return true;
					}

					if (next == 'o' || next == 'O') {
						advance(pos + gs);
						scan_octal_number(into);
						return true;
					}

					// try to parse an octal number
					if (is_octal_digit(next) && scan_octal_number(into, false))
						return true;
				}
			}
			case L'1':
			case L'2':
			case L'3':
			case L'4':
			case L'5':
			case L'6':
			case L'7':
			case L'8':
			case L'9':
				scan_decimal_number(into);
				return true;
			case L':':
				into.emplace_token<tokens::colon_token>(location());
				advance(pos);
				break;
			case L';':
				into.emplace_token<tokens::semicolon_token>(location());
				advance(pos);
				break;
			case L'<': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == L'=') {
						advance(pos + gs);
						into.emplace_token<tokens::less_eq_token>(loc);
						return true;
					}

					if (next == L'<') {
						wchar_t ltnext{};
						auto ggs = next_code_point(ltnext, pos + gs);
						if (ggs > 0) {
							if (ltnext == L'<') {
								scan_conflict_marker(into);
								return true;
							}

							if (ltnext == L'=') {
								advance(pos + gs + ggs);
								into.emplace_token<
									tokens::double_less_eq_token>(loc);
								return true;
							}
						}

						advance(pos + gs);
						into.emplace_token<tokens::double_less_token>(loc);
						return true;
					}

					if (is_alpha(next)) {
						if (scan_jsx_token(into)) {
							return true;
						}
					}
				}

				advance(pos);
				into.emplace_token<tokens::less_token>(loc);
				return true;
			}
			case L'=': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == L'=') {
						wchar_t eqnext{};
						auto ggs = next_code_point(eqnext, pos + gs);
						if (ggs > 0) {
							if (eqnext == L'=') {
								wchar_t eenext{};
								auto gggs = next_code_point(eenext);

								if (gggs > 0 && eenext == L'=') {
									scan_conflict_marker(into);
									return true;
								}

								advance(pos + gs + ggs);
								into.emplace_token<tokens::triple_eq_token>(
									loc);
								return true;
							}
						}

						advance(pos + gs);
						into.emplace_token<tokens::double_eq_token>(loc);
						return true;
					}

					if (next == L'>') {
						advance(pos + gs);
						into.emplace_token<tokens::eq_greater_token>(loc);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::eq_token>(loc);
				return true;
			}
			case L'>': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == L'=') {
						advance(pos + gs);
						into.emplace_token<tokens::greater_eq_token>(loc);
						return true;
					}

					if (next == L'>') {
						wchar_t gtnext{};
						auto ggs = next_code_point(gtnext, pos + gs);
						if (ggs > 0) {
							if (gtnext == L'>') {
								scan_conflict_marker(into);
								return true;
							}

							if (gtnext == L'=') {
								advance(pos + gs + ggs);
								into.emplace_token<
									tokens::double_greater_eq_token>(loc);
								return true;
							}
						}

						advance(pos + gs);
						into.emplace_token<tokens::double_greater_token>(loc);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::greater_token>(loc);
				return true;
			}
			case L'?': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					wchar_t qnext{};
					auto ggs = next_code_point(qnext, pos + gs);

					if (next == L'.' && (ggs > 0) && !is_decimal_digit(qnext)) {
						advance(pos + gs);
						into.emplace_token<tokens::question_dot_token>(loc);
						return true;
					}

					if (next == L'?') {
						if ((ggs > 0) && qnext == L'=') {
							advance(pos + gs + ggs);
							into.emplace_token<
								tokens::double_question_eq_token>(loc);
							return true;
						}

						advance(pos + gs);
						into.emplace_token<tokens::double_question_token>(loc);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::question_token>(loc);
				return true;
			}
			case L'[':
				into.emplace_token<tokens::open_bracket_token>(location());
				advance(pos);
				break;
			case L']':
				into.emplace_token<tokens::close_bracket_token>(location());
				advance(pos);
				break;
			case L'^': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if ((gs > 0) && next == L'=') {
					advance(pos + gs);
					into.emplace_token<tokens::caret_eq_token>(loc);
					return true;
				}

				advance(pos);
				into.emplace_token<tokens::caret_token>(loc);
				return true;
			}
			case L'{':
				into.emplace_token<tokens::open_brace_token>(location());
				advance(pos);
				break;
			case L'|': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == L'=') {
						advance(pos + gs);
						into.emplace_token<tokens::bar_eq_token>(loc);
						return true;
					}

					if (next == L'|') {
						wchar_t gtnext{};
						auto ggs = next_code_point(gtnext, pos + gs);
						if (ggs > 0) {
							if (gtnext == L'|') {
								scan_conflict_marker(into);
								return true;
							}

							if (gtnext == L'=') {
								advance(pos + gs + ggs);
								into.emplace_token<tokens::double_bar_eq_token>(
									loc);
								return true;
							}
						}

						advance(pos + gs);
						into.emplace_token<tokens::double_bar_token>(loc);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::bar_token>(loc);
				return true;
			}
			case L'}':
				into.emplace_token<tokens::close_brace_token>(location());
				advance(pos);
				break;
			case L'~':
				into.emplace_token<tokens::tilde_token>(location());
				advance(pos);
				break;
			case L'@':
				into.emplace_token<tokens::at_token>(location());
				advance(pos);
				break;
			case L'\\': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == L'u') {
						wchar_t ucnext{};
						auto ggs = next_code_point(ucnext, pos + gs);
						if (ggs > 0 && ucnext == L'{') {
							scan_unicode_escape(into, 1, true, false);
							return true;
						}

						scan_unicode_escape(into, 4, false, false);
					}
				}

				throw invalid_character(loc);
			}
			case L'#': {
				wchar_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == '!') {
						throw misplaced_shebang(loc);
					}

					advance(pos);
					if (try_scan_identifier(into, true)) {
						return true;
					}
				}

				throw invalid_character(loc);
			}
			default:
				if (try_scan_identifier(into)) {
					return true;
				}

				throw invalid_character(location());
		}
	}
}

source_location lexer::location() const {
	// get the column
	auto column = gpos_.offset - gpos_.line.line_start_offset;
	return source_location{source_, gpos_.line.current_line_number, column,
						   gpos_.offset};
}
