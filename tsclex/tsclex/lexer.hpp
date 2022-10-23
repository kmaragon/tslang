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
#include <optional>
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

		void advance_line() {
			++line.current_line_number;
			line.line_start_offset = offset;
		}
	};

	constexpr void advance(std::size_t by = 1) {
		gpos_.offset += by;
		buffer_offset_ += by;
	}

	// read more data from the stream into the buffer
	std::size_t next_code_point(wchar_t& into, std::size_t look_forward = 0);

	// read the next token from the stream
	bool scan(token& into);
	void scan_shebang(std::size_t shebang_offset, token& into);
	void scan_string(token& into);
	void scan_string_template(token& into);
	void scan_line_comment(token& into);
	void scan_multiline_comment(token& into, bool is_jsdoc);
	void scan_binary_number(token& into);
	bool scan_octal_number(token& into, bool throw_on_invalid = true);
	void scan_decimal_number(token& into);
	void scan_hex_number(token& into);
	void scan_conflict_marker(token& into);
	bool scan_jsx_token(token& into);
	void scan_unicode_escape(token& into,
							 std::size_t min_size,
							 bool scan_as_many_as_possible,
							 bool can_have_separators);

	// consider unicode and is identifier start
	bool try_scan_identifier(token& into, bool is_private = false);

	static constexpr bool is_decimal_digit(wchar_t ch);
	static constexpr bool is_octal_digit(wchar_t ch);
	static constexpr bool is_hex_digit(wchar_t ch);
	static constexpr bool is_alpha(wchar_t ch);

	source_location location() const;

	std::istream& stream_;
	std::shared_ptr<source> source_;

	// input buffer
	std::string rbuffer_;
	std::size_t buffer_offset_;

	// output buffer
	std::wstring wbuffer_;

	position_t gpos_;

	const iterator end_;
	bool cr_;
};

}  // namespace tscc::lex