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

#pragma once

#include <istream>
#include <optional>
#include <unordered_map>
#include <vector>
#include "error.hpp"

namespace tscc::lex {

// forward declaration of token
class token;

// TODO maybe move this
/**
 * \brief The TS language variant version
 */
enum class language_version : std::uint8_t {
	es3 = 0,
	es5 = 1,
	es2015 = 2,
	es2016 = 3,
	es2017 = 4,
	es2018 = 5,
	es2019 = 6,
	es2020 = 7,
	es2021 = 8,
	es2022 = 9,
	es_next = 99,
	json = 100,
	latest = es_next
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
	lexer(std::istream& stream,
		  std::shared_ptr<source> stream_metadata,
		  language_version version = language_version::latest);

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
	static std::array<char32_t, 512> unicode_es3_identifier_start;
	static std::array<char32_t, 684> unicode_es3_identifier_part;
	static std::array<char32_t, 740> unicode_es5_identifier_start;
	static std::array<char32_t, 856> unicode_es5_identifier_part;
	static std::array<char32_t, 1218> unicode_esnext_identifier_start;
	static std::array<char32_t, 1426> unicode_esnext_identifier_part;

	using tokfactory = void (*)(token& into, const source_location& location);
	static std::unordered_map<std::u32string, tokfactory> keyword_lookup;

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
	std::size_t next_code_point(char32_t& into, std::size_t look_forward = 0);

	// read the next token from the stream
	void scan_line_into_wbuffer(bool trim = true);

	bool scan(token& into);
	void scan_shebang(std::size_t shebang_offset, token& into);
	void scan_string(token& into);
	std::size_t scan_escape_sequence(char32_t& into, std::size_t skip = 0);
	void scan_string_template(token& into);
	void scan_line_comment(std::size_t comment_offset, token& into);
	void scan_multiline_comment(token& into, bool is_jsdoc);
	void scan_binary_token(token& into);
	std::size_t scan_binary_or_octal_number(long long& into, std::size_t base, std::size_t skip = 0);
	bool scan_octal_token(token& into, bool throw_on_invalid = true);
	void scan_decimal_token(token& into);
	void scan_big_integer_token(token& into);
	void scan_hex_token(token& into);
	void scan_conflict_marker(token& into);
	bool scan_jsx_token(token& into);
	void append_wbuffer(char32_t ch);

	std::size_t scan_hex_number(long long& into,
									std::size_t min_size,
									bool scan_as_many_as_possible,
									bool can_have_separators,
									std::size_t skip = 0);
	std::size_t scan_octal_number(long long& into, bool bail_on_decimal, std::size_t skip = 0);
	std::size_t scan_binary_number(long long& into, std::size_t skip = 0);

	// must scan one value or throw
	std::size_t scan_unicode_escape_into_wbuffer(std::size_t min_size,
												 bool scan_as_many_as_possible,
												 bool can_have_separators);


	// consider unicode and is identifier start
	void scan_identifier(token& into, bool is_private = false);

	static constexpr bool is_decimal_digit(char32_t ch);
	static constexpr long long decimal_value(char32_t ch);
	static constexpr bool is_octal_digit(char32_t ch);
	static constexpr bool is_hex_digit(char32_t ch);
	static constexpr bool is_alpha(char32_t ch);
	constexpr bool is_identifier_part(char32_t ch, bool is_jsx = false);
	constexpr bool is_identifier_start(char32_t ch);

	source_location location() const;

	std::istream& stream_;
	std::shared_ptr<source> source_;

	// input buffer
	std::string rbuffer_;
	std::size_t buffer_offset_;

	// output buffer
	std::u32string wbuffer_;
	std::vector<std::u32string> multiline_buffer_;
	position_t gpos_;
	const iterator end_;
	bool pnewline_;

	// force identifier rather than keyword after special token
	bool force_identifier_;

	const language_version vers_;
};

}  // namespace tscc::lex