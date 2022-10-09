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

#pragma once

#include <istream>
#include "source_location.hpp"

namespace tscc::lex {

// forward declaration of token
class token;

/**
 * \brief A generic exception while lexing
 */
class lex_error : public std::exception {
public:
	lex_error(const source_location& location) noexcept;

	/**
	 * \brief Get the location where the error occurred
	 */
	const source_location& location() const noexcept;

private:
	source_location location_;
};

/**
 * @brief The lexer that produces a stream of tokens from a stream of bytes
 *
 * The lexer is a simple contract that simply provides iterators. The iterators
 * are mutating of the actual lexer. So calling begin() multiple times will get
 * different tokens. As such, it is meant to be used one time to extract a
 * single iterator pair and that range can then be used to get a stream of
 * tokens out of a std::istream of chars.
 */
class lexer {
	static constexpr std::size_t buffer_size = 4096;

public:
	class iterator {
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = const token;
		using pointer_type = value_type*;
		using reference = value_type&;
		using iterator_category = std::input_iterator_tag;

		iterator(const iterator&) = default;
		iterator(iterator&&) noexcept = default;

		iterator& operator=(const iterator&) = default;
		iterator& operator=(iterator&&) noexcept = default;

		reference operator*() const;
		pointer_type operator->() const;

		iterator& operator++();
		iterator operator++(int);

		bool operator==(const iterator& other) const;
		bool operator!=(const iterator& other) const;

	private:
		iterator(lexer* source);

		std::shared_ptr<token> token_;
		lexer* lexer_;

		friend class lexer;
	};

	/**
	 * @brief Construct a lexer
	 * @param stream
	 * @param stream_metadata
	 */
	lexer(std::istream& stream, std::shared_ptr<source> stream_metadata);

	// disable copy / move
	lexer(const lexer&) = delete;

	/**
	 * @brief Get an iterator to the tokens in the stream
	 */
	iterator begin();

	/**
	 * @brief Get an end iterator for the tokens in the stream
	 */
	iterator end();

private:
	struct position_t {
		struct line_t {
			std::size_t current_line_number;
			std::size_t line_start_offset;

			constexpr line_t() noexcept
				: current_line_number(0), line_start_offset(0) {}
		} line;

		std::size_t offset;

		constexpr position_t() noexcept : offset(0) {}

		constexpr position_t& operator+=(const position_t& other) noexcept {
			auto original_offset = offset;

			offset += other.offset;
			line.current_line_number += other.line.current_line_number;
			if (other.line.current_line_number) {
				std::ptrdiff_t offset_line_delta =
					static_cast<std::ptrdiff_t>(other.offset) -
					static_cast<std::ptrdiff_t>(other.line.line_start_offset);

				line.line_start_offset = offset - offset_line_delta;
			}

			return *this;
		}

		void advance_line() {
			line.current_line_number++;
			line.line_start_offset = offset;
		}
	};

	enum class parse_state {
		// the initial state when first parsing
		initial,

		// first line, first character - hash - may be a shebang
		first_line_hash,

		// a shebang
		shebang,

		// any token
		scan_token,
	};

	// read more data from the stream into the buffer
	bool buffer_more();

	// read the next token from the stream
	bool read_next_token(token& into);

	source_location location() const;

	// handler functions
	constexpr bool process_initial(char c,
								   position_t& pos,
								   token& into,
								   bool eof);
	constexpr bool process_shebang(char c,
								   position_t& pos,
								   token& into,
								   bool eof);
	constexpr bool process_scan_token(char c,
									  position_t& pos,
									  token& into,
									  bool eof);

	template <bool (lexer::*processor)(char, position_t&, token&, bool)>
	constexpr bool scan_next(token& into);

	std::istream& stream_;
	std::shared_ptr<source> source_;

	std::string buffer_;
	std::size_t buffer_offset_;

	position_t gpos_;
	parse_state pstate_;

	const iterator end_;
	bool cr_;
};

}  // namespace tscc::lex