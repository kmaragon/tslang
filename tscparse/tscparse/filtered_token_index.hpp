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

#include <algorithm>
#include <optional>
#include <tsclex/token.hpp>
#include <vector>

namespace tscc::parse {

namespace detail {

template <typename... TokenTypes>
struct allowed_token_types_t {};

template <>
struct allowed_token_types_t<> {
	static constexpr bool allowed(const lex::token& t) noexcept {
		return false;
	}
};

template <typename T, typename... TokenTypes>
struct allowed_token_types_t<T, TokenTypes...> {
	static constexpr bool allowed(const tscc::lex::token& t) noexcept {
		return t.is<T>() || allowed_token_types_t<TokenTypes...>::allowed(t);
	}
};

template <typename... TokenTypes>
bool token_is_one_of(const lex::token& t) noexcept {
	return allowed_token_types_t<TokenTypes...>::allowed(t);
}

}  // namespace detail

/**
 * \brief Sorted index of filtered tokens for efficient location-based lookup
 *
 * Maintains a sorted list of tokens matching the specified types.
 * Supports binary search for O(log n) lookup by source location.
 */
template <typename RefType, typename... AllowedTokenTypes>
class filtered_token_index {
public:
	/**
	 * \brief Add a token to the index
	 */
	template <typename... Args>
	void emplace(lex::token tok, Args&&... args) {
		if (!detail::token_is_one_of<AllowedTokenTypes...>(tok)) {
			throw std::invalid_argument(
				"Token type not allowed in this filtered index");
		}

		tokens_.emplace_back(std::move(tok), std::forward<Args>(args)...);

		if (tokens_.size() > 1) {
			const auto& prev = tokens_[tokens_.size() - 2];
			const auto& curr = tokens_.back();
			if (compare_locations(curr.location(), prev.location()) < 0) {
				is_sorted_ = false;
			}
		}
	}

	/**
	 * \brief Finalize the index (sort if needed)
	 *
	 * Should be called after all tokens have been added.
	 * Makes the index ready for efficient queries.
	 */
	void finalize() {
		if (!is_sorted_) {
			std::sort(tokens_.begin(), tokens_.end(),
					  [](const RefType& a, const RefType& b) {
						  return compare_locations(a.location(), b.location()) <
								 0;
					  });
			is_sorted_ = true;
		}
	}

	/**
	 * \brief Find token at exact location
	 */
	std::optional<RefType> find_at(const lex::source_location& loc) const {
		ensure_sorted();

		auto it = std::lower_bound(
			tokens_.begin(), tokens_.end(), loc,
			[](const RefType& ref, const lex::source_location& loc) {
				return compare_locations(ref.location(), loc) < 0;
			});

		if (it != tokens_.end() && locations_equal(it->location(), loc)) {
			return *it;
		}
		return std::nullopt;
	}

	/**
	 * \brief Find tokens within a range
	 */
	std::vector<RefType> find_in_range(const lex::source_location& start,
									   const lex::source_location& end) const {
		ensure_sorted();

		auto start_it = std::lower_bound(
			tokens_.begin(), tokens_.end(), start,
			[](const RefType& ref, const lex::source_location& loc) {
				return compare_locations(ref.location(), loc) < 0;
			});

		auto end_it = std::upper_bound(
			tokens_.begin(), tokens_.end(), end,
			[](const lex::source_location& loc, const RefType& ref) {
				return compare_locations(loc, ref.location()) < 0;
			});

		return std::vector<RefType>(start_it, end_it);
	}

	/**
	 * \brief Find nearest token before a location
	 */
	std::optional<RefType> find_before(const lex::source_location& loc) const {
		ensure_sorted();

		auto it = std::lower_bound(
			tokens_.begin(), tokens_.end(), loc,
			[](const RefType& ref, const lex::source_location& loc) {
				return compare_locations(ref.location(), loc) < 0;
			});

		if (it != tokens_.begin()) {
			--it;
			return *it;
		}
		return std::nullopt;
	}

	/**
	 * \brief Find nearest token after a location
	 */
	std::optional<RefType> find_after(const lex::source_location& loc) const {
		ensure_sorted();

		auto it = std::upper_bound(
			tokens_.begin(), tokens_.end(), loc,
			[](const lex::source_location& loc, const RefType& ref) {
				return compare_locations(loc, ref.location()) < 0;
			});

		if (it != tokens_.end()) {
			return *it;
		}
		return std::nullopt;
	}

	/**
	 * \brief Get all tokens (for iteration)
	 */
	const std::vector<RefType>& all() const {
		ensure_sorted();
		return tokens_;
	}

private:
	std::vector<RefType> tokens_;
	bool is_sorted_ = true;

	void ensure_sorted() const {
		if (!is_sorted_) {
			const_cast<filtered_token_index*>(this)->finalize();
		}
	}

	static int compare_locations(const lex::source_location& a,
								 const lex::source_location& b) {
		if (a.line() < b.line())
			return -1;
		if (a.line() > b.line())
			return 1;
		if (a.column() < b.column())
			return -1;
		if (a.column() > b.column())
			return 1;
		return 0;
	}

	static bool locations_equal(const lex::source_location& a,
								const lex::source_location& b) {
		return a.line() == b.line() && a.column() == b.column();
	}
};

}  // namespace tscc::parse