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

bool lexer::scan_shebang(std::size_t shebang_offset, tscc::lex::token& into) {
	wchar_t first{};
	auto shebang_location = location();
	advance(shebang_offset);

	// skip any whitespace
	while (true) {
		auto nc = next_code_point(first);
		if (!nc) {
			throw expected_command(shebang_location);
		}

		advance(nc);
		if (!std::iswspace(first))
			break;
	}

	wbuffer_.reserve(buffer_size);
	wbuffer_.assign(&first, 1);

	// read the command until a eof or an eol
	while (true) {
		auto nc = next_code_point(first);
		if (!nc) {
			// trim any whitespace off of the end
			std::size_t end = wbuffer_.size();
			while (std::iswspace(wbuffer_[end - 1])) {
				end--;
			}

			wbuffer_.erase(end);
			into.emplace_token<tokens::shebang_token>(shebang_location,
													  std::move(wbuffer_));
			return true;
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
			into.emplace_token<tokens::shebang_token>(shebang_location,
													  std::move(wbuffer_));
			return true;
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
				return scan_shebang(pos + gs, into);
			}
		}

		return false;
	}
}

source_location lexer::location() const {
	// get the column
	auto column = gpos_.offset - gpos_.line.line_start_offset;
	return source_location{source_, gpos_.line.current_line_number, column,
						   gpos_.offset};
}
