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
#include "tokens/shebang_command.hpp"
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

	if (!lexer_->read_next_token(*token_))
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
	  pstate_(parse_state::initial),
	  end_(this),
	  cr_(false) {}

// private function template definition
template <bool (lexer::*processor)(char, lexer::position_t&, token&, bool)>
constexpr bool lexer::scan_next(token& into) {
	position_t local_position{};
	auto mark_offset = [&]() {
		local_position.offset++;
		gpos_ += local_position;
		buffer_offset_ += local_position.offset;
	};

	for (; true; local_position.offset++) {
		bool eof = false;

		// see if we are on the second to last character
		if ((local_position.offset + buffer_offset_ + 1) >= buffer_.size()) {
			if (!buffer_more()) {
				if ((local_position.offset + buffer_offset_) >= buffer_.size())
					return false;
				eof = true;
			}
		}

		char c = buffer_[local_position.offset + buffer_offset_];

		// if our prior character was a carriage return or if the current
		// character is a newline, process the newline in our line / column
		// accounting. We'll fall through regardless to let the individual
		// rule handle what to do with it.
		if (cr_ || c == '\n') {
			if (c == '\n') {
				local_position.advance_line();
			} else {
				local_position.offset--;
				if (((this)->*(processor))('\r', local_position, into, eof)) {
					mark_offset();
					return true;
				}

				local_position.offset++;
			}

			cr_ = false;
		}

		// if we got a carriage return, we'll treat it as a newline
		// if there's a newline after though, we'll treat the two together
		// as a single line
		if (c == '\r') {
			cr_ = true;
			if (!eof)
				continue;
		}

		if (((this)->*(processor))(c, local_position, into, eof)) {
			mark_offset();
			return true;
		}

		// if we're at the end of file and the handler
		// didn't return true, fail with a premature end
		if (eof) {
			return false;
		}
	}
}

constexpr bool lexer::process_initial(char c,
									  position_t& pos,
									  token& into,
									  bool eof) {
	auto line_number =
		pos.line.current_line_number + gpos_.line.current_line_number;

	// handle shebang only valid on the first line
	if (line_number == 0) {
		// if we already got a hash on the first line as the initial token
		// check for the !, without it, that's an error
		if (pstate_ == parse_state::first_line_hash) {
			if (c == '!') {
				// we have a shebang. Mark the token start
				pstate_ = parse_state::shebang;
				into.emplace_token<tokens::shebang_token>(location());
				return true;
			} else {
				stream_.setstate(std::ios::failbit);
				throw unexpected_token(location());
			}
		} else if (c == '#' && pstate_ == parse_state::initial) {
			// we got a first line hash - it might be a shebang
			pstate_ = parse_state::first_line_hash;
			return false;
		}
	}

	pstate_ = parse_state::scan_token;
	return process_scan_token(c, pos, into, eof);
}

constexpr bool lexer::process_shebang(char c,
									  position_t& pos,
									  token& into,
									  bool eof) {
	auto bc = buffer_[buffer_offset_];
	if (std::isspace(bc)) {
		if (c == '\n' || eof) {
			throw expected_command(location());
		}

		if (std::isspace(c)) {
			pos.offset++;
			gpos_ += pos;
			buffer_offset_ += pos.offset;

			pos = position_t{};
			pos.offset--;
			return false;
		}
	}

	if (c == '\n' || eof) {
		auto end = buffer_offset_ + pos.offset - (eof ? 0 : 1);
		while (std::isspace(buffer_[end]))
			--end;
		++end;

		auto command = buffer_.substr(buffer_offset_, end);
		into.emplace_token<tokens::shebang_command>(location(),
													std::move(command));
		return true;
	}

	return false;
}

constexpr bool lexer::process_scan_token(char c,
										 position_t& pos,
										 token& into,
										 bool eof) {
	if (std::isspace(c)) {
		pos.offset++;
		gpos_ += pos;
		buffer_offset_ += pos.offset;

		pos = position_t{};
		pos.offset--;
		return false;
	}

	// TODO: update
	return false;
}

lexer::iterator lexer::begin() {
	lexer::iterator result(this);
	++result;
	return result;
}

lexer::iterator lexer::end() {
	return end_;
}

bool lexer::buffer_more() {
	// first move the content that is already consumed
	std::size_t current_size = buffer_.size();
	if (buffer_offset_) {
		memmove(buffer_.data(), buffer_.data() + buffer_offset_,
				buffer_.size() - buffer_offset_);
		current_size -= buffer_offset_;
		buffer_offset_ = 0;
	}

	// ensure we have at least buffer_size available in teh buffer
	buffer_.resize(current_size + buffer_size);
	auto read = stream_.readsome(buffer_.data() + current_size, buffer_.size());
	if (read > 0) {
		buffer_.resize(current_size + read);
		return true;
	}

	buffer_.resize(current_size);
	return false;
}

bool lexer::read_next_token(tscc::lex::token& into) {
	try {
		switch (pstate_) {
			case parse_state::initial:
			case parse_state::first_line_hash:
				return scan_next<&lexer::process_initial>(into);
			case parse_state::shebang:
				return scan_next<&lexer::process_shebang>(into);
			case parse_state::scan_token:
				return scan_next<&lexer::process_scan_token>(into);
		}
	} catch (const lex_error&) {
		stream_.setstate(std::ios_base::failbit);
		throw;
	}
}

source_location lexer::location() const {
	// get the column
	auto column = gpos_.offset - gpos_.line.line_start_offset;
	return source_location{source_, gpos_.line.current_line_number, column,
						   gpos_.offset};
}
