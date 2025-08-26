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

#include "lexer.hpp"
#include <array>
#include <cassert>
#include <cmath>
#include <cstring>
#include "error/expected_command.hpp"
#include "error/hexidecimal_digit_expected.hpp"
#include "error/invalid_character.hpp"
#include "error/invalid_identifier.hpp"
#include "error/misplaced_shebang.hpp"
#include "error/multiple_separators_not_allowed.hpp"
#include "error/no_jsx_closing_tag.hpp"
#include "error/separators_not_allowed_here.hpp"
#include "error/unexpected_end_of_text.hpp"
#include "error/unicode_value_out_of_range.hpp"
#include "error/unterminated_multiline_comment.hpp"
#include "error/unterminated_string_literal.hpp"
#include "error/unterminated_unicode_escape_sequence.hpp"
#include "private/unicode_tables.hpp"
#include "token.hpp"
#include "tsccore/utf8.hpp"

using namespace tscc::lex;

lex_error::lex_error(const source_location& location) noexcept
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

lexer::lexer(std::istream& stream,
			 std::shared_ptr<source> stream_metadata,
			 language_version version)
	: stream_(stream),
	  source_(std::move(stream_metadata)),
	  buffer_offset_(0),
	  end_(this),
	  pnewline_(false),
	  force_identifier_(false),
	  vers_(version) {
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

std::unordered_map<std::u32string, lexer::versioned_keyword>
	lexer::
		keyword_lookup{{U"abstract",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::abstract_token>(
								location);
						}, language_version::es3}},
					   {U"accessor",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::accessor_token>(
								location);
						}, language_version::es3}},
					   {U"any",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::any_token>(location);
						}, language_version::es3}},
					   {U"as",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::as_token>(location);
						}, language_version::es3}},
					   {U"asserts",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::asserts_token>(location);
						}, language_version::es3}},
					   {U"assert",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::assert_token>(location);
						}, language_version::es3}},
					   {U"async",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::async_token>(location);
						}, language_version::es2015}},
					   {U"await",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::await_token>(location);
						}, language_version::es2015}},
					   {U"bigint",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::bigint_token>(location);
						}, language_version::es3}},
					   {U"boolean",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::boolean_token>(location);
						}, language_version::es3}},
					   {U"break",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::break_token>(location);
						}, language_version::es3}},
					   {U"case",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::case_token>(location);
						}, language_version::es3}},
					   {U"catch",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::catch_token>(location);
						}, language_version::es3}},
					   {U"class",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::class_token>(location);
						}, language_version::es2015}},
					   {U"continue",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::continue_token>(
								location);
						}, language_version::es3}},
					   {U"const",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::const_token>(location);
						}, language_version::es2015}},
					   {U"constructor",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::constructor_token>(
								location);
						}, language_version::es3}},
					   {U"debugger",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::debugger_token>(
								location);
						}, language_version::es3}},
					   {U"declare",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::declare_token>(location);
						}, language_version::es3}},
					   {U"default",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::default_token>(location);
						}, language_version::es2015}},
					   {U"delete",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::delete_token>(location);
						}, language_version::es3}},
					   {U"do",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::do_token>(location);
						}, language_version::es3}},
					   {U"else",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::else_token>(location);
						}, language_version::es3}},
					   {U"enum",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::enum_token>(location);
						}, language_version::es3}},
					   {U"export",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::export_token>(location);
						}, language_version::es2015}},
					   {U"extends",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::extends_token>(location);
						}, language_version::es2015}},
					   {U"false",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::false_token>(location);
						}, language_version::es3}},
					   {U"finally",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::finally_token>(location);
						}, language_version::es3}},
					   {U"for",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::for_token>(location);
						}, language_version::es3}},
					   {U"from",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::from_token>(location);
						}, language_version::es2015}},
					   {U"function",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::function_token>(
								location);
						}, language_version::es3}},
					   {U"get",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::get_token>(location);
						}, language_version::es5}},
					   {U"global",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::global_token>(location);
						}, language_version::es3}},
					   {U"if",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::if_token>(location);
						}, language_version::es3}},
					   {U"implements",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::implements_token>(
								location);
						}, language_version::es3}},
					   {U"import",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::import_token>(location);
						}, language_version::es2015}},
					   {U"in",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::in_token>(location);
						}, language_version::es3}},
					   {U"infer",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::infer_token>(location);
						}, language_version::es3}},
					   {U"instanceof",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::instanceof_token>(
								location);
						}, language_version::es3}},
					   {U"interface",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::interface_token>(
								location);
						}, language_version::es3}},
					   {U"intrinsic",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::intrinsic_token>(
								location);
						}, language_version::es3}},
					   {U"is",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::is_token>(location);
						}, language_version::es3}},
					   {U"keyof",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::keyof_token>(location);
						}, language_version::es3}},
					   {U"let",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::let_token>(location);
						}, language_version::es2015}},
					   {U"module",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::module_token>(location);
						}, language_version::es3}},
					   {U"namespace",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::namespace_token>(
								location);
						}, language_version::es3}},
					   {U"never",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::never_token>(location);
						}, language_version::es3}},
					   {U"new",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::new_token>(location);
						}, language_version::es3}},
					   {U"null",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::null_token>(location);
						}, language_version::es3}},
					   {U"number",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::number_token>(location);
						}, language_version::es3}},
					   {U"of",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::of_token>(location);
						}, language_version::es2015}},
					   {U"object",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::object_token>(location);
						}, language_version::es3}},
					   {U"package",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::package_token>(location);
						}, language_version::es3}},
					   {U"private",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::private_token>(location);
						}, language_version::es3}},
					   {U"protected",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::protected_token>(
								location);
						}, language_version::es3}},
					   {U"public",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::public_token>(location);
						}, language_version::es3}},
					   {U"override",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::override_token>(
								location);
						}, language_version::es3}},
					   {U"out",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::out_token>(location);
						}, language_version::es3}},
					   {U"readonly",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::readonly_token>(
								location);
						}, language_version::es3}},
					   {U"require",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::require_token>(location);
						}, language_version::es3}},
					   {U"return",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::return_token>(location);
						}, language_version::es3}},
					   {U"satisfies",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::satisfies_token>(
								location);
						}, language_version::es3}},
					   {U"set",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::set_token>(location);
						}, language_version::es5}},
					   {U"static",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::static_token>(location);
						}, language_version::es2015}},
					   {U"string",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::string_token>(location);
						}, language_version::es3}},
					   {U"super",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::super_token>(location);
						}, language_version::es2015}},
					   {U"switch",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::switch_token>(location);
						}, language_version::es3}},
					   {U"symbol",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::symbol_token>(location);
						}, language_version::es2015}},
					   {U"this",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::this_token>(location);
						}, language_version::es3}},
					   {U"throw",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::throw_token>(location);
						}, language_version::es3}},
					   {U"true",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::true_token>(location);
						}, language_version::es3}},
					   {U"try",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::try_token>(location);
						}, language_version::es3}},
					   {U"type",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::type_token>(location);
						}, language_version::es3}},
					   {U"typeof",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::typeof_token>(location);
						}, language_version::es3}},
					   {U"undefined",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::undefined_token>(
								location);
						}, language_version::es3}},
					   {U"unique",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::unique_token>(location);
						}, language_version::es3}},
					   {U"unknown",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::unknown_token>(location);
						}, language_version::es3}},
					   {U"using",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::using_token>(location);
						}, language_version::es2022}},
					   {U"var",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::var_token>(location);
						}, language_version::es3}},
					   {U"void",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::void_token>(location);
						}, language_version::es3}},
					   {U"while",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::while_token>(location);
						}, language_version::es3}},
					   {U"with",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::with_token>(location);
						}, language_version::es3}},
					   {U"yield",
						{[](token& into, const source_location& location) {
							into.emplace_token<tokens::yield_token>(location);
						}, language_version::es2015}}};

inline std::size_t lexer::next_code_point(char32_t& into,
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

		into = (static_cast<char32_t>(chr0 & 0x1f) << 6) |
			   static_cast<char32_t>(chr1 & 0x3f);
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

		into = (static_cast<char32_t>(chr0 & 0x0f) << 12) |
			   (static_cast<char32_t>(chr1 & 0x3f) << 6) |
			   static_cast<char32_t>(chr2 & 0x3f);
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

		into = (static_cast<char32_t>(chr0 & 0x07) << 18) |
			   (static_cast<char32_t>(chr1 & 0x3f) << 12) |
			   (static_cast<char32_t>(chr2 & 0x3f) << 6) |
			   static_cast<char32_t>(chr3 & 0x3f);
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

		into = (static_cast<char32_t>(chr0 & 0x03) << 24) |
			   (static_cast<char32_t>(chr1 & 0x3f) << 18) |
			   (static_cast<char32_t>(chr2 & 0x3f) << 12) |
			   (static_cast<char32_t>(chr3 & 0x3f) << 6) |
			   static_cast<char32_t>(chr4 & 0x3f);
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

		into = (static_cast<char32_t>(chr0 & 0x01) << 30) |
			   (static_cast<char32_t>(chr1 & 0x3f) << 24) |
			   (static_cast<char32_t>(chr2 & 0x3f) << 18) |
			   (static_cast<char32_t>(chr3 & 0x3f) << 12) |
			   (static_cast<char32_t>(chr4 & 0x3f) << 6) |
			   static_cast<char32_t>(chr5 & 0x3f);
		return 6;
	}

	return chr0;
}

void lexer::scan_shebang(std::size_t shebang_offset, token& into) {
	auto shebang_location = location();
	advance(shebang_offset);

	scan_line_into_wbuffer();

	if (wbuffer_.empty())
		throw expected_command(shebang_location);

	into.emplace_token<tokens::shebang_token>(shebang_location, wbuffer_);
}

void lexer::scan_string(token& into) {
	auto string_location = location();

	char32_t quote_end;
	char32_t first{};

	advance(next_code_point(quote_end));
	wbuffer_.clear();

	// read the string until an end quote
	while (true) {
		auto nc = next_code_point(first);
		if (!nc) {
			throw unterminated_string_literal(string_location);
		}

		advance(nc);
		if (first == quote_end) {
			break;
		}

		switch (first) {
			case '\r':
			case '\n':
				throw unterminated_string_literal(string_location);
			case '\\': {
				char32_t second{};
				auto nnc = next_code_point(second);
				if (nnc) {
					if (second == '\r') {
						char32_t third{};
						auto nnnc = next_code_point(third);
						if (nc && third == '\n') {
							advance(nnc + nnnc);
							gpos_.advance_line();
							goto scan_next_char;
						}

						advance(nc + nnc);
						goto scan_next_char;
					}

					if (second == '\n') {
						advance(nnc);
						gpos_.advance_line();
						goto scan_next_char;
					}
				}

				advance(scan_escape_sequence(first));
			}
		}

		append_wbuffer(first);
	scan_next_char:
	}

	into.emplace_token<tokens::constant_value_token>(
		string_location, wbuffer_, static_cast<char>(quote_end));
}

void lexer::scan_template_string_part(token& into) {
	auto string_location = location();

	char32_t quote_start;
	char32_t first{};

	auto nc = next_code_point(quote_start);
	// if we are at the end of the string
	if (quote_start == U'`') {
		into.emplace_token<tokens::interpolated_string_end_token>(location());
		advance(nc);
		context_stack_.pop_back();
		return;
	} else if (quote_start == U'$') {
		char32_t second;
		auto gc = next_code_point(second, nc);
		if (gc && second == '{') {
			into.emplace_token<tokens::template_start_token>(location(), false);
			advance(nc + gc);
			context_stack_.push_back(
				std::make_pair(in_template_expression, location()));
			return;
		}
	}

	wbuffer_.clear();

	// read the com
	while (true) {
		nc = next_code_point(first);
		if (!nc) {
			throw unterminated_string_literal(
				context_stack_.front().second.location);
		}

		// we have an end quote on not the first character so emit the string
		// part without closing out the context
		if (first == U'`') {
			into.emplace_token<tokens::interpolated_string_chunk_token>(
				location(), std::move(wbuffer_));
			return;
		}

		if (first == '$') {
			// otherwise we consume and advance
			char32_t second;
			auto gc = next_code_point(second, nc);
			if (gc && second == '{') {
				// emit the chunk and pick up the start
				// on the next round - so don't advance
				into.emplace_token<tokens::interpolated_string_chunk_token>(
					source_location(), std::move(wbuffer_));
				return;
			}

			advance(nc);
			append_wbuffer(first);
		} else if (first == '\r') {
			// special case - \r\n normalizes back to \n
			char32_t second;
			auto gc = next_code_point(second, nc);
			if (gc && second == '\n') {
				// consume the \r and throw it away - catch the newline
				// below
				first = second;
				advance(nc + gc);
				gpos_.advance_line();
				append_wbuffer(second);
				continue;
			}
		} else if (first == '\\') {
			advance(nc + scan_escape_sequence(first));
			append_wbuffer(first);
		} else {
			if (first == '\n')
				gpos_.advance_line();
			advance(nc);
			append_wbuffer(first);
		}
	}
}

std::size_t lexer::scan_escape_sequence(char32_t& into, std::size_t skip) {
	auto gc = next_code_point(into, skip);
	if (!gc)
		return 0;

	if (into == 'u' || into == 'U') {
		bool is_curly_braced = false;
		char32_t check_curly;
		auto cc = next_code_point(check_curly, skip + gc);
		if (cc && check_curly == '{') {
			gc += cc;
			is_curly_braced = true;
		}

		long long ucfirst;
		auto scanned = scan_hex_number(ucfirst, is_curly_braced ? 1 : 4,
									   is_curly_braced, false, gc + skip);

		if (!scanned) {
			return gc - is_curly_braced ? cc : 0;
		}

		if (is_curly_braced) {
			cc = next_code_point(check_curly, skip + gc + scanned);
			if (!cc) {
				throw unexpected_end_of_text(location());
			}

			if (check_curly != '}') {
				throw unterminated_unicode_escape_sequence(location() + gc);
			}

			// Validate the Unicode code point
			if (ucfirst > 0x10FFFF) {
				throw unicode_value_out_of_range(location());
			}

			into = ucfirst;
			return gc + scanned + cc;
		}

		// Validate the Unicode code point
		if (ucfirst > 0xFFFF) {
			throw unicode_value_out_of_range(location());
		}

		if ((ucfirst >> 11) != 0x1b) {
			into = ucfirst;
			return gc + scanned;
		}

		char32_t next;
		auto nnc = next_code_point(next, gc + skip + scanned);
		if (!nnc || next != '\\') {
			into = ucfirst;
			return gc + scanned;
		}

		char32_t checku;
		auto checkuc = next_code_point(checku, gc + skip + scanned + nnc);
		if (!checkuc || (checku != 'u' && checku != 'U')) {
			into = ucfirst;
			return gc + scanned;
		}

		try {
			long long ucsecond;
			auto nscanned = scan_hex_number(
				ucsecond, 4, false, false, gc + skip + scanned + nnc + checkuc);
			if (ucsecond <= 0xFFFF && nscanned && (ucsecond >> 10) == 0x37) {
				into =
					(((ucfirst & 0x3ff) << 10) | (ucsecond & 0x3ff)) + 0x10000;
				return gc + scanned + nnc + checkuc + nscanned;
			}
		} catch (...) {
			// ignore errors in the subsequent section at this scope
		}

		into = ucfirst;
		return gc + scanned;
	}

	if (into == 'x' || into == 'X') {
		// handle 1 byte hex identifier
		long long hexfirst;
		auto scanned = scan_hex_number(hexfirst, 2, false, false, gc + skip);

		if (!scanned || hexfirst > 0xFF) {
			return gc + skip;
		}

		into = hexfirst;
		return scanned;
	}

	if (into == '0') {
		// handle either octal or binary
		char32_t next;
		auto nnc = next_code_point(next, gc + skip);
		if (nnc == 'b' || nnc == 'B') {
			long long number;
			auto scanned = scan_binary_number(number, false, gc + nnc + skip);
			if (scanned && number < 0x7f000000ll) {
				into = static_cast<char32_t>(number);
				return scanned;
			}

			return gc + skip;
		}

		long long number;
		auto scanned = scan_octal_number(number, false, false, skip);
		if (scanned && number < 0x7f000000ll) {
			into = static_cast<char32_t>(number);
			return scanned;
		}

		return gc + skip;
	}

	if (is_octal_digit(into)) {
		long long number;
		auto scanned = scan_octal_number(number, false, false, gc + skip);
		if (scanned && number < 0x7f000000ll) {
			into = static_cast<char32_t>(number);
			return scanned;
		}

		return gc + skip;
	}

	switch (into) {
		case 'b':
			into = '\b';
			break;
		case 'f':
			into = '\f';
			break;
		case 'n':
			into = '\n';
			break;
		case 'r':
			into = '\r';
			break;
		case 't':
			into = '\t';
			break;
		case 'v':
			into = '\v';
			break;
		default:
			break;
	}

	return gc + skip;
}

void lexer::scan_line_comment(std::size_t comment_offset, token& into) {
	auto comment_location = location();
	advance(comment_offset);

	scan_line_into_wbuffer();

	into.emplace_token<tokens::comment_token>(comment_location,
											  std::move(wbuffer_));
}

void lexer::scan_multiline_comment(token& into, bool is_jsdoc) {
	auto comment_location = location();

	std::size_t multiline_index = 0;
	if (multiline_buffer_.size() < 1) {
		multiline_buffer_.resize(1);
	}

	char32_t first{};
	while (true) {
		auto nc = next_code_point(first);
		if (!nc) {
			throw unterminated_multiline_comment(comment_location);
		}

		advance(nc);
		if (first == '\n') {
			// trim any whitespace off of the end
			std::size_t end = wbuffer_.size();
			while (std::iswspace(wbuffer_[end - 1])) {
				end--;
			}

			wbuffer_.erase(end);
			gpos_.advance_line();

			if (multiline_buffer_.size() == multiline_index) {
				std::size_t new_size = multiline_buffer_.size() >= 1024
										   ? multiline_buffer_.size() + 1024
										   : multiline_buffer_.size() * 2;
				multiline_buffer_.resize(new_size);
			}
			multiline_buffer_[multiline_index++].assign(wbuffer_);
			wbuffer_.clear();
			continue;
		}

		if (first == '*') {
			char32_t second{};
			auto nnc = next_code_point(second);
			if (nnc && second == '/') {
				advance(nnc);
				break;
			}
		}

		append_wbuffer(first);
	}

	if (!wbuffer_.empty()) {
		if (multiline_buffer_.size() == multiline_index) {
			std::size_t new_size = multiline_buffer_.size() >= 1024
									   ? multiline_buffer_.size() + 1024
									   : multiline_buffer_.size() * 2;
			multiline_buffer_.resize(new_size);
		}

		multiline_buffer_[multiline_index++].assign(wbuffer_);
		wbuffer_.clear();
	}

	if (is_jsdoc) {
		into.emplace_token<tokens::jsdoc_token>(
			comment_location,
			std::span(multiline_buffer_.begin(), multiline_index));
		return;
	}

	into.emplace_token<tokens::multiline_comment_token>(
		comment_location,
		std::span(multiline_buffer_.begin(), multiline_index));
}

void lexer::scan_binary_token(token& into) {
	tscc_big_int number;
	auto scanned = scan_binary_number(number, true);
	if (!scanned) {
		throw invalid_identifier(location());
	}

	auto loc = location();
	advance(scanned);

	bool is_bigint = check_and_consume_bigint_suffix();
	auto size = is_bigint ? tokens::integer_size::big_int
						  : tokens::integer_size::standard;

	into.emplace_token<tokens::constant_value_token>(
		loc, number, tokens::integer_base::binary, size);
}

bool lexer::scan_octal_token(token& into, bool throw_on_invalid) {
	tscc_big_int number;
	auto scanned = scan_octal_number(number, true, true);
	if (!scanned) {
		if (throw_on_invalid)
			throw invalid_identifier(location());
		return false;
	}

	auto loc = location();
	advance(scanned);

	bool is_bigint = check_and_consume_bigint_suffix();
	auto size = is_bigint ? tokens::integer_size::big_int
						  : tokens::integer_size::standard;

	into.emplace_token<tokens::constant_value_token>(
		loc, number, tokens::integer_base::octal, size);
	return true;
}

std::size_t lexer::scan_binary_or_octal_number(tscc_big_int& into,
											   std::size_t base,
											   std::size_t skip) {
	auto number_location = location();

	std::size_t nc = 0;
	std::size_t taken = 0;

	char32_t first{};
	while (true) {
		nc = next_code_point(first, skip);
		if (!nc) {
			return taken;
		}

		if (!is_octal_digit(first))
			break;

		advance(nc);
		break;
	}

	throw "implementation truncated";
}

void lexer::scan_decimal_token(token& into) {
	auto number_location = location();

	tscc_big_int number_part = 0;
	std::size_t nc = 0;
	bool is_first_character = true;
	bool last_was_separator = false;

	auto last_separator = location();

	char32_t first{};
	while (true) {
		nc = next_code_point(first);
		if (!nc) {
			into.emplace_token<tokens::constant_value_token>(
				number_location, number_part, tokens::integer_base::decimal);
			break;
		}

		if (!is_decimal_digit(first)) {
			// separators are allowed here
			if (first == U'_') {
				if (last_was_separator) {
					throw multiple_separators_not_allowed(number_location);
				}

				if (is_first_character) {
					throw separators_not_allowed_here(location());
				}

				last_separator = location();
				last_was_separator = true;
				advance(nc);
				continue;
			}

			break;
		}

		is_first_character = false;
		advance(nc);
		number_part = (number_part * 10) + decimal_value(first);
		last_was_separator = false;
	}

	if (last_was_separator) {
		throw separators_not_allowed_here{last_separator};
	}

	if (nc && first == U'.') {
		advance(nc);

		// parse as a decimal
		long double numerator = 0;
		long long numerator_e = 0;

		while (true) {
			nc = next_code_point(first);
			if (!nc || !is_decimal_digit(first)) {
				if (nc) {
					if (first == U'.') {
						throw invalid_identifier(number_location);
					}
					if (first == U'_') {
						throw separators_not_allowed_here{location()};
					}
				}
				break;
			}

			advance(nc);
			numerator = (numerator * 10) + decimal_value(first);
			--numerator_e;
		}

		auto value = static_cast<long double>(number_part) +
					 (numerator * std::pow(10.0L, numerator_e));
		if (nc && (first == 'e' || first == 'E')) {
			advance(nc);
			bool is_upper = first == 'E';

			nc = next_code_point(first);
			if (!nc) {
				throw invalid_identifier(number_location);
			}

			long long exponent = 1;

			switch (first) {
				case U'_':
					throw separators_not_allowed_here{location()};
				case U'-':
					exponent = -1;
				case U'+':
					advance(nc);

					nc = next_code_point(first);
					if (!nc || !is_decimal_digit(first)) {
						throw invalid_identifier(number_location);
					}

					exponent *= decimal_value(first);
					break;
				default:
					if (!is_decimal_digit(first)) {
						throw invalid_identifier(number_location);
					}
					exponent = decimal_value(first);
			}

			advance(nc);

			// this is a double with an e value
			while (true) {
				nc = next_code_point(first);
				if (!nc || !is_decimal_digit(first)) {
					if (nc && first == U'_') {
						throw separators_not_allowed_here{location()};
					}

					break;
				}

				advance(nc);
				exponent = (exponent * 10) + decimal_value(first);
			}

			into.emplace_token<tokens::constant_value_token>(
				number_location, value, static_cast<int>(exponent), is_upper);
		} else {
			into.emplace_token<tokens::constant_value_token>(number_location,
															 value);
		}

		return;
	}

	if (nc && (first == 'e' || first == 'E')) {
		advance(nc);
		bool is_upper = first == 'E';

		nc = next_code_point(first);
		if (!nc) {
			throw invalid_identifier(number_location);
		}

		long long sign = 1;
		long long exponent = 1;

		switch (first) {
			case '-':
				sign = -1;
				exponent = -1;
				[[fallthrough]];
			case '+':
				advance(nc);

				nc = next_code_point(first);
				if (!nc || !is_decimal_digit(first)) {
					throw invalid_identifier(number_location);
				}

				exponent *= decimal_value(first);
				break;
			default:
				if (!is_decimal_digit(first)) {
					throw invalid_identifier(number_location);
				}
				exponent = decimal_value(first);
		}

		advance(nc);

		// this is an integer with an e value - convert to long double
		while (true) {
			nc = next_code_point(first);
			if (!nc || !is_decimal_digit(first)) {
				if (nc && first == U'_') {
					throw separators_not_allowed_here{location()};
				}

				break;
			}

			advance(nc);
			exponent = (exponent * 10) + (sign * decimal_value(first));
		}

		into.emplace_token<tokens::constant_value_token>(
			number_location, static_cast<long double>(number_part),
			static_cast<int>(exponent), is_upper);
		return;
	}

	if (check_and_consume_bigint_suffix()) {
		into.emplace_token<tokens::constant_value_token>(
			number_location, number_part, tokens::integer_base::decimal,
			tokens::integer_size::big_int);
	} else {
		into.emplace_token<tokens::constant_value_token>(
			number_location, number_part, tokens::integer_base::decimal);
	}
}

void lexer::scan_hex_token(token& into) {
	auto number_location = location();
	tscc_big_int number_part = 0;
	auto scanned = scan_hex_number(number_part, 1, true, true);

	if (!scanned) {
		throw invalid_identifier(number_location);
	}

	advance(scanned);

	bool is_bigint = check_and_consume_bigint_suffix();
	auto size = is_bigint ? tokens::integer_size::big_int
						  : tokens::integer_size::standard;

	into.emplace_token<tokens::constant_value_token>(
		number_location, number_part, tokens::integer_base::hex, size);
}

bool lexer::scan_conflict_marker(token& into) {
	std::size_t count = 1;
	auto start = location();

	char32_t st{};
	auto skip = next_code_point(st);

	while (true) {
		char32_t ch{};
		auto nc = next_code_point(ch, skip);
		if (!nc) {
			return false;
		}
		if (ch != st) {
			if (count < 7) {
				return false;
			}

			break;
		}

		skip += nc;
		++count;
	}

	// if the start is <<<<<<< or >>>>>>>, it needs to be followed by a space
	if (st == U'<' || st == U'>') {
		char32_t next{};
		auto nc = next_code_point(next, skip);
		if (!nc || !std::iswspace(next)) {
			return false;
		}

		skip += nc;
	}

	advance(skip);
	wbuffer_.clear();
	scan_line_into_wbuffer(true);

	// at this point, we will certainly return true;
	into.emplace_token<tokens::conflict_marker_trivia_token>(
		start, static_cast<char>(st), std::move(wbuffer_));
	return true;
}

bool lexer::scan_jsx_token(token& into) {
	if (source_->language_variant() == ts_language_variant::ts) {
		return false;
	}

	char32_t angle;
	auto nc = next_code_point(angle);

	// we only got here because we already have the < and one character from the
	// identifier
	assert(nc && angle == U'<');

	auto at = nc;
	auto start = location() + nc;

	// scan the the jsx identifier
	wbuffer_.clear();
	while (true) {
		char32_t next{};
		nc = next_code_point(next, at);
		if (!nc) {
			// end of file before in the middle of what would have been an
			// identifier
			return false;
		}

		if (std::iswspace(next))
			break;

		if (next == U'>') {
			// we got a close tag. This is an identifier
			advance(at);
			into.emplace_token<tokens::jsx_element_start_token>(location(),
																wbuffer_);

			context_stack_.push_back(std::make_pair(
				in_jsx_element, stack_entry{start, std::move(wbuffer_)}));
			return true;
		}

		if (!is_identifier_part(next, true)) {
			return false;
		}

		wbuffer_.push_back(next);
		at += nc;
	}

	// if we got here, we may have a jsx token - we just need to validate before
	// we continue on
	auto element_name_end = at;
	while (true) {
		char32_t next{};
		nc = next_code_point(next, at);
		if (!nc) {
			return false;
		}

		at += nc;
		if (std::iswspace(next)) {
			continue;
		}

		if (is_identifier_start(next) || next == U'>') {
			break;
		}

		if (next == U'/') {
			char32_t test{};
			auto gc = next_code_point(test, at);
			if (!gc) {
				return false;
			}

			if (test == U'>') {
				break;
			}
		}
	}

	// this is a jsx element
	into.emplace_token<tokens::jsx_element_start_token>(location(), wbuffer_);

	// add case insensitive buffer to the context
	context_stack_.push_back(std::make_pair(
		in_jsx_element, stack_entry{start, std::move(wbuffer_)}));
	advance(element_name_end);
	return true;
}

void lexer::scan_jsx_element_part(token& into) {
	auto element_start = context_stack_.back().second.location;

	while (true) {
		char32_t next{};

		auto nc = next_code_point(next);
		if (next == U'\n') {
			gpos_.advance_line();
			advance(nc);
			continue;
		}

		if (is_identifier_start(next)) {
			// we have an attribute
			break;
		}

		if (std::iswspace(next)) {
			advance(nc);
			continue;
		}

		if (next == U'/') {
			char32_t test{};
			auto gc = next_code_point(test, nc);
			if (!gc) {
				throw unexpected_end_of_text(element_start);
			}

			if (test == U'>') {
				into.emplace_token<tokens::jsx_self_closing_token>(location());
				context_stack_.pop_back();
				advance(nc + gc);
				return;
			}

			continue;
		}

		if (next == U'>') {
			into.emplace_token<tokens::jsx_element_end_token>(location());
			advance(nc);
			context_stack_.push_back(std::make_pair(in_jsx_text, location()));
			return;
		}

		throw invalid_identifier(element_start);
	}

	// We should be at an attribute name
	auto start_name = location();
	wbuffer_.clear();
	bool end_of_name = false;
	while (true) {
		char32_t next{};

		auto nc = next_code_point(next);
		if (!nc) {
			throw unexpected_end_of_text(element_start);
		}

		if (next == U'=') {
			// this is the end of the tag - switch to the text
			advance(nc);
			into.emplace_token<tokens::jsx_attribute_name_token>(
				start_name, std::move(wbuffer_));
			context_stack_.push_back(
				std::make_pair(in_jsx_attribute, location()));
			return;
		}

		if (std::iswspace(next)) {
			advance(nc);
			if (next == U'\n') {
				gpos_.advance_line();
			}
			end_of_name = true;
			continue;
		}

		if (!is_identifier_part(next, true)) {
			throw invalid_identifier(location());
		}

		if (end_of_name) {
			// we add the name but change no context because it's just
			// a name with no value
			into.emplace_token<tokens::jsx_attribute_name_token>(
				start_name, std::move(wbuffer_));
			return;
		}

		advance(nc);
		wbuffer_.push_back(next);
	}
}

void lexer::scan_jsx_attribute_part(token& into) {
	char32_t next{};
	auto nc = next_code_point(next);

	while (std::iswspace(next)) {
		if (next == U'\n') {
			gpos_.advance_line();
		} else {
			advance(nc);
		}
		nc = next_code_point(next);
	}

	auto attribute_start = location();

	// next contains the first character in the attribute
	if (next == U'"' || next == U'\'') {
		auto quote_char = next;
		advance(nc);

		wbuffer_.clear();
		while (true) {
			nc = next_code_point(next);
			if (!nc) {
				throw unexpected_end_of_text(attribute_start);
			}

			advance(nc);
			if (next == quote_char) {
				// end of the quote
				into.emplace_token<tokens::jsx_attribute_value_token>(
					location(), wbuffer_, (char)quote_char);
				context_stack_.pop_back();
				return;
			}

			wbuffer_.push_back(next);
		}
	}

	if (next != U'{') {
		into.emplace_token<tokens::jsx_attribute_value_start_token>(location());
		context_stack_.push_back(std::make_pair(in_jsx_expression, location()));
		advance(nc);
		return;
	}

	advance(nc);
	context_stack_.push_back(std::make_pair(in_jsx_expression, location()));
	into.emplace_token<tokens::jsx_attribute_value_start_token>(location());
}

void lexer::scan_jsx_text_part(token& into) {
	// start off and see if we immediately end or immediately start an
	// expression
	char32_t next{};
	auto nc = next_code_point(next);
	if (next == U'{') {
		into.emplace_token<tokens::template_start_token>(location(), true);
		context_stack_.push_back(std::make_pair(in_jsx_expression, location()));
		advance(nc);
		return;
	}

	if (next == U'<') {
		// we are immediately starting either a close element or start element
		char32_t test{};
		auto gc = next_code_point(test, nc);
		if (gc && test == '/') {
			// we got a closing tag
			auto start = location();
			advance(gc + nc);

			nc = next_code_point(next);

			// read the identifier
			wbuffer_.clear();
			while (true) {
				nc = next_code_point(next);
				if (!nc) {
					throw unexpected_end_of_text(start);
				}

				if (!is_identifier_part(next, true))
					break;

				advance(nc);
				wbuffer_.push_back(next);
			}

			if (next != U'>') {
				throw invalid_character(location());
			}

			// pop the in_jsx_text from the stack
			context_stack_.pop_back();

			// so we should be back to the element
			assert(!context_stack_.empty());
			assert(context_stack_.back().first == in_jsx_element);

			// then we can get the tag name
			into.emplace_token<tokens::jsx_element_close_token>(location(),
																wbuffer_);

			if (wbuffer_ != context_stack_.back().second.text) {
				throw no_jsx_closing_tag(
					location(), utf8_encode(context_stack_.back().second.text));
			}

			context_stack_.pop_back();
			advance(nc);
			return;
		}

		auto start_location = location() + nc;
		if (!scan_jsx_token(into)) {
			throw invalid_identifier(start_location);
		}

		return;
	}

	wbuffer_.clear();
	while (true) {
		nc = next_code_point(next);
		if (!nc) {
			throw unexpected_end_of_text(context_stack_.back().second.location);
		}

		if (next == U'\n') {
			gpos_.advance_line();
		} else if (next == U'\r') {
			char32_t check{};
			auto gc = next_code_point(check);
			if (gc && next == U'\n') {
				advance(nc);
				next = check;
				gpos_.advance_line();
			}
		} else if (next == '{') {
			// we have a token starting
			into.emplace_token<tokens::jsx_text_token>(location(),
													   std::move(wbuffer_));
			return;
		} else if (next == '<') {
			into.emplace_token<tokens::jsx_text_token>(location(),
													   std::move(wbuffer_));
			return;
		}

		advance(nc);
		wbuffer_.push_back(next);
	}
}

bool lexer::try_scan_regex(token& into) {
	char32_t next{};

	// scan the slash
	auto skip = scan_escape_sequence(next);
	wbuffer_.clear();

	// TODO fully parse

	return false;
}

void lexer::append_wbuffer(char32_t ch) {
	if (wbuffer_.capacity() == wbuffer_.size()) {
		wbuffer_.reserve(wbuffer_.size() + buffer_size);
	}
	wbuffer_.push_back(ch);
}

std::size_t lexer::scan_hex_number(tscc_big_int& into,
								   std::size_t min_size,
								   bool scan_as_many_as_possible,
								   bool can_have_separators,
								   std::size_t skip) {
	char32_t ch{};
	std::size_t total_skip = skip;
	std::size_t hex_digits = 0;
	bool prior_was_separator = false;
	bool at_first_char = true;
	char32_t value = 0;
	size_t nc = 0;

	// Read hex digits until we have enough or hit a non-hex character
	while (true) {
		nc = next_code_point(ch, total_skip);
		if (!nc) {
			break;
		}

		// Skip optional separators if allowed
		if (ch == '_') {
			if (!can_have_separators || at_first_char) {
				throw separators_not_allowed_here(location() + total_skip);
			}

			if (prior_was_separator) {
				throw multiple_separators_not_allowed(location() + total_skip);
			}

			prior_was_separator = true;
			total_skip += nc;
			continue;
		}

		at_first_char = false;
		prior_was_separator = false;

		// Check if we have a valid hex digit
		if (!is_hex_digit(ch)) {
			break;
		}

		// Convert hex digit to value
		if (ch >= '0' && ch <= '9') {
			value = (value << 4) | (ch - '0');
		} else if (ch >= 'A' && ch <= 'F') {
			value = (value << 4) | (ch - 'A' + 10);
		} else if (ch >= 'a' && ch <= 'f') {
			value = (value << 4) | (ch - 'a' + 10);
		} else if (ch >= 0xff10 && ch <= 0xff19) {
			value = (value << 4) | (ch - 0xff10);
		} else if (ch >= 0xff21 && ch <= 0xff26) {
			value = (value << 4) | (ch - 0xff21 + 10);
		} else if (ch >= 0xff41 && ch <= 0xff46) {
			value = (value << 4) | (ch - 0xff41 + 10);
		}

		hex_digits++;
		total_skip += nc;

		// If we have enough digits and aren't scanning for more, stop
		if (hex_digits >= min_size && !scan_as_many_as_possible) {
			break;
		}
	}

	if (prior_was_separator) {
		throw separators_not_allowed_here(location() + (total_skip - nc));
	}

	// If we didn't get enough hex digits, return 0 to indicate failure
	if (hex_digits < min_size) {
		throw hexidecimal_digit_expected(location() + total_skip);
	}

	into = value;
	return total_skip - skip;
}

std::size_t lexer::scan_octal_number(tscc_big_int& into,
									 bool bail_on_decimal,
									 bool can_have_separators,
									 std::size_t skip) {
	auto number_location = location();
	std::size_t nc = 0;
	std::size_t taken = 0;
	bool prior_was_separator = false;
	bool at_first_char = true;
	into = 0;

	char32_t first{};
	while (true) {
		nc = next_code_point(first, skip + taken);
		if (!nc) {
			break;
		}

		if (!is_octal_digit(first)) {
			// Skip optional separators if allowed
			if (first == '_') {
				if (!can_have_separators || at_first_char) {
					throw separators_not_allowed_here(location() + skip +
													  taken);
				}

				if (prior_was_separator) {
					throw multiple_separators_not_allowed(location() + skip +
														  taken);
				}

				prior_was_separator = true;
				taken += nc;
				continue;
			}

			if (bail_on_decimal && is_decimal_digit(first))
				return 0;
			break;
		}

		prior_was_separator = false;
		at_first_char = false;
		into = (into * 8) + decimal_value(first);
		taken += nc;
	}

	if (prior_was_separator) {
		throw separators_not_allowed_here(location() + (skip + taken - nc));
	}

	return taken;
}

std::size_t lexer::scan_binary_number(tscc_big_int& into,
									  bool can_have_separators,
									  std::size_t skip) {
	auto number_location = location();
	std::size_t nc = 0;
	std::size_t taken = 0;
	bool prior_was_separator = false;
	bool at_first_char = true;
	into = 0;

	char32_t first{};
	while (true) {
		nc = next_code_point(first, skip + taken);
		if (!nc) {
			break;
		}

		if (first == '_') {
			if (!can_have_separators || at_first_char) {
				throw separators_not_allowed_here(location() + skip + taken);
			}

			if (prior_was_separator) {
				throw multiple_separators_not_allowed(location() + skip +
													  taken);
			}

			prior_was_separator = true;
			taken += nc;
			continue;
		}

		at_first_char = false;
		prior_was_separator = false;
		if (first != U'0' && first != U'1')
			break;

		into = (into * 2) + (first - U'0');
		taken += nc;
	}

	if (prior_was_separator) {
		throw separators_not_allowed_here(location() + (skip + taken - nc));
	}

	return taken;
}

std::size_t lexer::scan_unicode_escape_into_wbuffer(
	std::size_t min_size,
	bool scan_as_many_as_possible,
	bool can_have_separators) {
	long long result;
	auto scanned = scan_hex_number(result, min_size, scan_as_many_as_possible,
								   can_have_separators);

	if (!scanned)
		throw invalid_identifier(location());

	advance(scanned);
	append_wbuffer(result);
	return scanned;
}

void lexer::scan_identifier(token& into, bool is_private) {
	wbuffer_.clear();
	auto identifier_start = location();

	while (true) {
		char32_t ch{};
		auto pos = next_code_point(ch);

		if (!pos)
			break;

		if (ch == L'\\') {
			char32_t next{};
			auto gs = next_code_point(next, pos);

			auto loc = location();
			if (gs > 0) {
				wbuffer_.clear();
				if (next == U'u') {
					char32_t ucnext{};
					auto ggs = next_code_point(ucnext, pos + gs);
					if (ggs > 0 && ucnext == U'{') {
						advance(pos + gs + ggs);

						auto scanned =
							scan_unicode_escape_into_wbuffer(1, true, false);
						if (!scanned)
							throw invalid_identifier(identifier_start);
						advance(scanned);
						ggs = next_code_point(ucnext);
						if (!is_identifier_part(
								wbuffer_[wbuffer_.size() - 1]) ||
							!ggs || ucnext != U'}') {
							throw invalid_character(loc);
						}

						advance(ggs);
						continue;
					}

					advance(pos + gs);
					auto scanned =
						scan_unicode_escape_into_wbuffer(4, false, false);
					if (!scanned)
						throw invalid_identifier(identifier_start);
					advance(scanned);
					if (!is_identifier_part(wbuffer_[wbuffer_.size() - 1])) {
						throw invalid_character(loc);
					}

					continue;
				}
			}

			throw invalid_character(loc);
		} else if (!is_identifier_part(ch)) {
			break;
		}

		append_wbuffer(ch);
		advance(pos);
	}

	if (wbuffer_.empty() || !is_identifier_start(wbuffer_[0])) {
		throw invalid_identifier(identifier_start);
	}

	if (!force_identifier_) {
		auto kit = keyword_lookup.find(wbuffer_);
		if (kit != keyword_lookup.end() && vers_ >= kit->second.min_version) {
			kit->second.factory(into, identifier_start);
			return;
		}
	}

	if (is_private)
		wbuffer_.insert(wbuffer_.begin(), 1, U'#');

	into.emplace_token<tokens::identifier_token>(identifier_start,
												 std::move(wbuffer_));
}

constexpr bool lexer::is_decimal_digit(char32_t ch) {
	return (ch >= U'0' && ch <= U'9') || (ch >= 0xff10 && ch <= 0xff19);
}

constexpr long long lexer::decimal_value(char32_t ch) {
	if (ch >= 0xff10) {
		return ch - 0xff10;
	}

	return ch - U'0';
}

constexpr bool lexer::is_octal_digit(char32_t ch) {
	return (ch >= U'0' && ch < U'8') || (ch >= 0xff10 && ch < 0xff18);
}

constexpr bool lexer::is_hex_digit(char32_t ch) {
	return (ch >= U'0' && ch <= U'8') || (ch >= U'A' && ch <= U'F') ||
		   (ch >= U'a' && ch <= U'f') || (ch >= 0xff21 && ch <= 0xff26) ||
		   (ch >= 0xff41 && ch <= 0xff46) || (ch >= 0xff10 && ch <= 0xff18);
}

constexpr bool lexer::is_alpha(char32_t ch) {
	return (ch >= U'A' && ch <= U'Z') || (ch >= U'a' && ch <= U'z') ||
		   (ch >= 0xff21 && ch <= 0xff3a) || (ch >= 0xff41 && ch <= 0xff5a);
}

constexpr bool lexer::is_identifier_part(char32_t ch, bool is_jsx) {
	// A-Za-z0-9
	if ((ch >= U'A' && ch <= U'Z') || (ch >= U'a' && ch <= U'z') ||
		(ch >= U'0' && ch <= U'9'))
		return true;

	if (ch == U'$' || ch == U'_')
		return true;

	if (is_jsx && (ch == '-' || ch == ':' || ch == '.'))
		return true;

	if (ch > 0x7f) {
		if (vers_ >= language_version::es2015) {
			auto fnd = std::lower_bound(
				detail::unicode_tables::esnext_identifier_part.begin(),
				detail::unicode_tables::esnext_identifier_part.end(), ch);
			return fnd !=
					   detail::unicode_tables::esnext_identifier_part.end() &&
				   *fnd == ch;
		}

		if (vers_ >= language_version::es5) {
			auto fnd = std::lower_bound(
				detail::unicode_tables::es5_identifier_part.begin(),
				detail::unicode_tables::es5_identifier_part.end(), ch);
			return fnd != detail::unicode_tables::es5_identifier_part.end() &&
				   *fnd == ch;
		}

		auto fnd = std::lower_bound(
			detail::unicode_tables::es3_identifier_part.begin(),
			detail::unicode_tables::es3_identifier_part.end(), ch);
		return fnd != detail::unicode_tables::es3_identifier_part.end() &&
			   *fnd == ch;
	}

	return false;
}

constexpr bool lexer::is_identifier_start(char32_t ch) {
	// A-Za-z0-9
	if ((ch >= U'A' && ch <= U'Z') || (ch >= U'a' && ch <= U'z'))
		return true;

	if (ch == U'$' || ch == U'_')
		return true;

	if (ch > 0x7f) {
		if (vers_ >= language_version::es2015) {
			auto fnd = std::lower_bound(
				detail::unicode_tables::esnext_identifier_start.begin(),
				detail::unicode_tables::esnext_identifier_start.end(), ch);
			return fnd !=
					   detail::unicode_tables::esnext_identifier_start.end() &&
				   *fnd == ch;
		}

		if (vers_ >= language_version::es5) {
			auto fnd = std::lower_bound(
				detail::unicode_tables::es5_identifier_start.begin(),
				detail::unicode_tables::es5_identifier_start.end(), ch);
			return fnd != detail::unicode_tables::es5_identifier_start.end() &&
				   *fnd == ch;
		}

		auto fnd = std::lower_bound(
			detail::unicode_tables::es3_identifier_start.begin(),
			detail::unicode_tables::es3_identifier_start.end(), ch);
		return fnd != detail::unicode_tables::es3_identifier_start.end() &&
			   *fnd == ch;
	}

	return false;
}

void lexer::scan_line_into_wbuffer(bool trim) {
	char32_t first{};

	// skip any whitespace
	while (true) {
		auto nc = next_code_point(first);
		if (!nc) {
			wbuffer_.clear();
			return;
		}

		advance(nc);

		// special case - did we just read a newline - which is
		// technically still whitespace but is already the end of the line
		if (first == '\n') {
			wbuffer_.clear();
			return;
		}

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

		append_wbuffer(first);
	}
}

bool lexer::scan(token& into) {
	// automatically reset one iteration flags after call
	struct reset_one_iteration_values {
		lexer* lexer_;
		reset_one_iteration_values(lexer* l) : lexer_(l) {}

		void skip() { lexer_ = nullptr; }

		void force_identifier() { force_identifier_ = true; }

		~reset_one_iteration_values() {
			if (lexer_ != nullptr) {
				lexer_->force_identifier_ = force_identifier_;
			}
		}

	private:
		bool force_identifier_ = false;
	};

	reset_one_iteration_values resetter{this};
	if (!context_stack_.empty()) {
		switch (context_stack_.back().first) {
			case in_template_literal:
				scan_template_string_part(into);
				return true;
			case in_jsx_element:
				scan_jsx_element_part(into);
				return true;
			case in_jsx_attribute:
				scan_jsx_attribute_part(into);
				return true;
			case in_jsx_text:
				scan_jsx_text_part(into);
				return true;
			default:
				break;
		}
	}

	while (true) {
		char32_t ch{};
		auto pos = next_code_point(ch);

		if (!pos) {
			// special case, if we were at a valid template point but in an
			// interpolated string, it might otherwise look like we're good but
			// we're not
			if (!context_stack_.empty()) {
				switch (context_stack_.front().first) {
					case in_template_literal:
					case in_template_expression:
					case in_nested_brace:
						throw unterminated_string_literal(
							context_stack_.front().second.location);
					case in_jsx_element:
					case in_jsx_text:
					case in_jsx_expression:
					case in_jsx_attribute:
						// For JSX contexts, use unexpected_end_of_text which is
						// more generic
						throw unexpected_end_of_text(
							context_stack_.front().second.location);
				}
			}
			return false;
		}

		// check for shebang
		if (gpos_.offset == 0 && ch == U'#') {
			char32_t next{};
			auto gs = next_code_point(next, pos);
			if ((gs > 0) && next == U'!') {
				scan_shebang(pos + gs, into);
				return true;
			}
		}

		if (ch == '\r') {
			char32_t next{};
			auto gs = next_code_point(next, pos);

			auto loc = location();
			if ((gs > 0) && next == L'\n') {
				pos += gs;
			}

			advance(pos);
			gpos_.advance_line();
			if (pnewline_)
				continue;

			pnewline_ = true;
			into.emplace_token<tokens::newline_token>(loc);
			return true;
		}

		if (ch == '\n') {
			auto loc = location();
			advance(pos);
			gpos_.advance_line();

			if (pnewline_)
				continue;

			pnewline_ = true;
			into.emplace_token<tokens::newline_token>(loc);
			return true;
		}

		pnewline_ = false;
		switch (ch) {
			case L'\t':	  // tab
			case 0x0b:	  // vertical tab
			case 0x0c:	  // form feed
			case U' ':	  // space
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
				resetter.skip();
				advance(pos);
				break;
			case U'!': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == U'=') {
						char32_t excnext{};
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
			case U'"':
			case U'\'':
				scan_string(into);
				return true;
			case U'`':
				context_stack_.push_back(
					std::make_pair(in_template_literal, location()));
				into.emplace_token<tokens::interpolated_string_start_token>(
					location());
				advance(pos);
				return true;
			case U'%': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if ((gs > 0) && next == U'=') {
					advance(pos + gs);
					into.emplace_token<tokens::percent_eq_token>(loc);
					return true;
				}

				advance(pos);
				into.emplace_token<tokens::percent_token>(loc);
				return true;
			}
			case U'&': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == U'=') {
						advance(pos + gs);
						into.emplace_token<tokens::ampersand_eq_token>(loc);
						return true;
					}

					if (next == U'&') {
						char32_t ampnext{};
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
			case U'(':
				into.emplace_token<tokens::open_paren_token>(location());
				advance(pos);
				return true;
			case U')':
				into.emplace_token<tokens::close_paren_token>(location());
				advance(pos);
				return true;
			case U'*': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == U'=') {
						advance(pos + gs);
						into.emplace_token<tokens::asterisk_eq_token>(loc);
						return true;
					}

					if (next == U'*') {
						char32_t astnext{};
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
			case U'+': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == U'+') {
						advance(pos + gs);
						into.emplace_token<tokens::double_plus_token>(loc);
						return true;
					}

					if (next == U'=') {
						advance(pos + gs);
						into.emplace_token<tokens::plus_eq_token>(loc);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::plus_token>(loc);
				return true;
			}
			case U',':
				into.emplace_token<tokens::comma_token>(location());
				advance(pos);
				return true;
			case U'-': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == U'-') {
						advance(pos + gs);
						into.emplace_token<tokens::double_minus_token>(loc);
						return true;
					}

					if (next == U'=') {
						advance(pos + gs);
						into.emplace_token<tokens::minus_eq_token>(loc);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::minus_token>(loc);
				return true;
			}
			case U'.': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == U'.') {
						char32_t dotnext{};
						auto ggs = next_code_point(dotnext, pos + gs);
						if (ggs > 0 && dotnext == '.') {
							advance(pos + gs + ggs);
							into.emplace_token<tokens::triple_dot_token>(loc);
							return true;
						}
					} else if (is_decimal_digit(next)) {
						scan_decimal_token(into);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::dot_token>(loc);
				return true;
			}
			case U'/': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == U'=') {
						advance(pos + gs);
						into.emplace_token<tokens::slash_eq_token>(loc);
						return true;
					}

					if (next == U'/') {
						scan_line_comment(pos + gs, into);
						return true;
					}

					if (next == U'*') {
						bool is_jsdoc = false;

						char32_t astnext{};
						auto ggs = next_code_point(astnext, pos + gs);
						if (ggs > 0 && astnext == '*') {
							char32_t verify_jsdoc_char;
							auto verify_gs = next_code_point(verify_jsdoc_char,
															 pos + gs + ggs);

							if (verify_gs && verify_jsdoc_char != '*') {
								gs += ggs;
								is_jsdoc = true;
							}
						}

						advance(pos + gs);
						scan_multiline_comment(into, is_jsdoc);
						return true;
					}

					if (try_scan_regex(into)) {
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::slash_token>(loc);
				return true;
			}
			case U'0': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == 'x' || next == 'X') {
						advance(pos + gs);
						scan_hex_token(into);
						return true;
					}

					if (next == 'b' || next == 'B') {
						advance(pos + gs);
						scan_binary_token(into);
						return true;
					}

					if (next == 'o' || next == 'O') {
						advance(pos + gs);
						scan_octal_token(into);
						return true;
					}

					// try to parse an octal number
					if (is_octal_digit(next)) {
						tscc_big_int octal_value;
						auto taken =
							scan_octal_number(octal_value, true, true, pos);
						if (taken) {
							advance(pos + gs + taken);
							bool is_bigint = check_and_consume_bigint_suffix();
							auto size = is_bigint
											? tokens::integer_size::big_int
											: tokens::integer_size::standard;
							into.emplace_token<tokens::constant_value_token>(
								location(), octal_value,
								tokens::integer_base::octal, size);
						} else {
							advance(pos);
							scan_decimal_token(into);
						}
						return true;
					}
				}
			}
			case U'1':
			case U'2':
			case U'3':
			case U'4':
			case U'5':
			case U'6':
			case U'7':
			case U'8':
			case U'9':
				scan_decimal_token(into);
				return true;
			case U':':
				into.emplace_token<tokens::colon_token>(location());
				advance(pos);
				return true;
			case U';':
				into.emplace_token<tokens::semicolon_token>(location());
				advance(pos);
				return true;
			case U'<': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == U'=') {
						advance(pos + gs);
						into.emplace_token<tokens::less_eq_token>(loc);
						return true;
					}

					if (next == U'<') {
						char32_t ltnext{};
						auto ggs = next_code_point(ltnext, pos + gs);
						if (ggs > 0) {
							if (ltnext == U'<') {
								if (scan_conflict_marker(into)) {
									return true;
								}
							}

							if (ltnext == U'=') {
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

					if (is_identifier_start(next) || next == U'>') {
						if (scan_jsx_token(into)) {
							return true;
						}
					}
				}

				advance(pos);
				into.emplace_token<tokens::less_token>(loc);
				return true;
			}
			case U'=': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == U'=') {
						char32_t eqnext{};
						auto ggs = next_code_point(eqnext, pos + gs);
						if (ggs > 0) {
							if (eqnext == U'=') {
								char32_t eenext{};
								auto gggs =
									next_code_point(eenext, pos + gs + ggs);

								if (gggs > 0 && eenext == U'=') {
									if (scan_conflict_marker(into)) {
										return true;
									}
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

					if (next == U'>') {
						advance(pos + gs);
						into.emplace_token<tokens::eq_greater_token>(loc);
						return true;
					}
				}

				advance(pos);
				into.emplace_token<tokens::eq_token>(loc);
				return true;
			}
			case U'>': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == U'=') {
						advance(pos + gs);
						into.emplace_token<tokens::greater_eq_token>(loc);
						return true;
					}

					if (next == U'>') {
						char32_t gtnext{};
						auto ggs = next_code_point(gtnext, pos + gs);
						if (ggs > 0) {
							if (gtnext == U'>') {
								if (scan_conflict_marker(into)) {
									return true;
								}

								char32_t ttnext{};
								auto tgs =
									next_code_point(ttnext, pos + gs + ggs);
								if (tgs > 0 && ttnext == '=') {
									advance(pos + gs + ggs + tgs);
									into.emplace_token<
										tokens::triple_greater_eq_token>(loc);
									return true;
								}

								advance(pos + gs + ggs);
								into.emplace_token<
									tokens::triple_greater_token>(loc);
								return true;
							}

							if (gtnext == U'=') {
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
			case U'?': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					char32_t qnext{};
					auto ggs = next_code_point(qnext, pos + gs);

					if (next == U'.' &&
						(ggs == 0 || (is_identifier_start(qnext) &&
									  !is_decimal_digit(qnext)))) {
						advance(pos + gs);
						into.emplace_token<tokens::question_dot_token>(loc);
						return true;
					}

					if (next == U'?') {
						if ((ggs > 0) && qnext == U'=') {
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
			case U'[':
				into.emplace_token<tokens::open_bracket_token>(location());
				advance(pos);
				return true;
			case U']':
				into.emplace_token<tokens::close_bracket_token>(location());
				advance(pos);
				return true;
			case U'^': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if ((gs > 0) && next == U'=') {
					advance(pos + gs);
					into.emplace_token<tokens::caret_eq_token>(loc);
					return true;
				}

				advance(pos);
				into.emplace_token<tokens::caret_token>(loc);
				return true;
			}
			case U'{':
				// special case for string interpolation
				if (!context_stack_.empty()) {
					if (context_stack_.back().first == in_template_expression ||
						context_stack_.back().first == in_nested_brace ||
						context_stack_.back().first == in_jsx_expression) {
						// increase the brace depth if we are in a template
						context_stack_.push_back(
							std::make_pair(in_nested_brace, location()));
					}
				}
				into.emplace_token<tokens::open_brace_token>(location());
				advance(pos);
				return true;
			case U'|': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == U'=') {
						advance(pos + gs);
						into.emplace_token<tokens::bar_eq_token>(loc);
						return true;
					}

					if (next == U'|') {
						char32_t gtnext{};
						auto ggs = next_code_point(gtnext, pos + gs);
						if (ggs > 0) {
							if (gtnext == U'|') {
								if (scan_conflict_marker(into)) {
									return true;
								}
							}

							if (gtnext == U'=') {
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
			case U'}':
				// special case for string interpolation
				if (!context_stack_.empty()) {
					if (context_stack_.back().first == in_nested_brace) {
						// if we're in a brace in the template, just pop it
						context_stack_.pop_back();
					} else if (context_stack_.back().first ==
								   in_template_expression ||
							   context_stack_.back().first ==
								   in_jsx_expression) {
						// otherwise if we are ending the template
						context_stack_.pop_back();

						// if we're in an element and we are in a jsx
						// expression, then we are undoubtedly in an attribute
						// in that element
						if (!context_stack_.empty() &&
							context_stack_.back().first == in_jsx_attribute) {
							into.emplace_token<
								tokens::jsx_attribute_value_end_token>(
								location());
							context_stack_.pop_back();
						} else {
							into.emplace_token<tokens::template_end_token>(
								location());
						}
						advance(pos);
						return true;
					}
				}
				into.emplace_token<tokens::close_brace_token>(location());
				advance(pos);
				return true;
			case U'~':
				into.emplace_token<tokens::tilde_token>(location());
				advance(pos);
				return true;
			case U'@':
				into.emplace_token<tokens::at_token>(location());
				advance(pos);

				resetter.force_identifier();
				return true;
			case U'#': {
				char32_t next{};
				auto gs = next_code_point(next, pos);

				auto loc = location();
				if (gs > 0) {
					if (next == '!') {
						throw misplaced_shebang(loc);
					}

					if (is_identifier_start(next)) {
						advance(pos);
						scan_identifier(into, true);
						return true;
					}
				}

				throw invalid_character(loc);
			}
			default:
				scan_identifier(into);
				return true;
		}
	}
}

bool lexer::check_and_consume_bigint_suffix() {
	char32_t next{};
	auto nc = next_code_point(next);
	if (nc && (next == U'n' || next == U'N')) {
		advance(nc);
		return true;
	}
	return false;
}

source_location lexer::location() const {
	// get the column
	auto column = gpos_.offset - gpos_.line.line_start_offset;
	return source_location{source_, gpos_.line.current_line_number, column,
						   gpos_.offset};
}
