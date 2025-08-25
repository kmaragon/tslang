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

#include <optional>
#include <variant>
#include "source_location.hpp"
#include "tokens/abstract_token.hpp"
#include "tokens/accessor_token.hpp"
#include "tokens/ampersand_eq_token.hpp"
#include "tokens/ampersand_token.hpp"
#include "tokens/any_token.hpp"
#include "tokens/as_token.hpp"
#include "tokens/assert_token.hpp"
#include "tokens/asserts_token.hpp"
#include "tokens/asterisk_eq_token.hpp"
#include "tokens/asterisk_token.hpp"
#include "tokens/async_token.hpp"
#include "tokens/at_token.hpp"
#include "tokens/await_token.hpp"
#include "tokens/bar_eq_token.hpp"
#include "tokens/bar_token.hpp"
#include "tokens/bigint_token.hpp"
#include "tokens/boolean_token.hpp"
#include "tokens/break_token.hpp"
#include "tokens/caret_eq_token.hpp"
#include "tokens/caret_token.hpp"
#include "tokens/case_token.hpp"
#include "tokens/catch_token.hpp"
#include "tokens/class_token.hpp"
#include "tokens/close_brace_token.hpp"
#include "tokens/close_bracket_token.hpp"
#include "tokens/close_paren_token.hpp"
#include "tokens/colon_token.hpp"
#include "tokens/comma_token.hpp"
#include "tokens/comment_token.hpp"
#include "tokens/conflict_marker_trivia_token.hpp"
#include "tokens/const_token.hpp"
#include "tokens/constant_value_token.hpp"
#include "tokens/constructor_token.hpp"
#include "tokens/continue_token.hpp"
#include "tokens/debugger_token.hpp"
#include "tokens/declare_token.hpp"
#include "tokens/default_token.hpp"
#include "tokens/delete_token.hpp"
#include "tokens/do_token.hpp"
#include "tokens/dot_token.hpp"
#include "tokens/double_ampersand_eq_token.hpp"
#include "tokens/double_ampersand_token.hpp"
#include "tokens/double_asterisk_eq_token.hpp"
#include "tokens/double_asterisk_token.hpp"
#include "tokens/double_bar_eq_token.hpp"
#include "tokens/double_bar_token.hpp"
#include "tokens/double_eq_token.hpp"
#include "tokens/double_greater_eq_token.hpp"
#include "tokens/double_greater_token.hpp"
#include "tokens/double_less_eq_token.hpp"
#include "tokens/double_less_token.hpp"
#include "tokens/double_minus_token.hpp"
#include "tokens/double_plus_token.hpp"
#include "tokens/double_question_eq_token.hpp"
#include "tokens/double_question_token.hpp"
#include "tokens/else_token.hpp"
#include "tokens/enum_token.hpp"
#include "tokens/eq_greater_token.hpp"
#include "tokens/eq_token.hpp"
#include "tokens/exclamation_eq_eq_token.hpp"
#include "tokens/exclamation_eq_token.hpp"
#include "tokens/exclamation_token.hpp"
#include "tokens/export_token.hpp"
#include "tokens/extends_token.hpp"
#include "tokens/false_token.hpp"
#include "tokens/finally_token.hpp"
#include "tokens/for_token.hpp"
#include "tokens/from_token.hpp"
#include "tokens/function_token.hpp"
#include "tokens/get_token.hpp"
#include "tokens/global_token.hpp"
#include "tokens/greater_eq_token.hpp"
#include "tokens/greater_token.hpp"
#include "tokens/identifier_token.hpp"
#include "tokens/if_token.hpp"
#include "tokens/implements_token.hpp"
#include "tokens/import_token.hpp"
#include "tokens/in_token.hpp"
#include "tokens/infer_token.hpp"
#include "tokens/instanceof_token.hpp"
#include "tokens/interface_token.hpp"
#include "tokens/interpolated_string_chunk_token.hpp"
#include "tokens/interpolated_string_end_token.hpp"
#include "tokens/interpolated_string_start_token.hpp"
#include "tokens/intrinsic_token.hpp"
#include "tokens/is_token.hpp"
#include "tokens/jsdoc_token.hpp"
#include "tokens/jsx_attribute_name_token.hpp"
#include "tokens/jsx_attribute_value_end_token.hpp"
#include "tokens/jsx_attribute_value_start_token.hpp"
#include "tokens/jsx_attribute_value_token.hpp"
#include "tokens/jsx_element_close_token.hpp"
#include "tokens/jsx_element_end_token.hpp"
#include "tokens/jsx_element_start_token.hpp"
#include "tokens/jsx_self_closing_token.hpp"
#include "tokens/jsx_text_token.hpp"
#include "tokens/keyof_token.hpp"
#include "tokens/less_eq_token.hpp"
#include "tokens/less_token.hpp"
#include "tokens/let_token.hpp"
#include "tokens/minus_eq_token.hpp"
#include "tokens/minus_token.hpp"
#include "tokens/module_token.hpp"
#include "tokens/multiline_comment_token.hpp"
#include "tokens/namespace_token.hpp"
#include "tokens/never_token.hpp"
#include "tokens/new_token.hpp"
#include "tokens/newline_token.hpp"
#include "tokens/null_token.hpp"
#include "tokens/number_token.hpp"
#include "tokens/object_token.hpp"
#include "tokens/of_token.hpp"
#include "tokens/open_brace_token.hpp"
#include "tokens/open_bracket_token.hpp"
#include "tokens/open_paren_token.hpp"
#include "tokens/out_token.hpp"
#include "tokens/override_token.hpp"
#include "tokens/package_token.hpp"
#include "tokens/percent_eq_token.hpp"
#include "tokens/percent_token.hpp"
#include "tokens/plus_eq_token.hpp"
#include "tokens/plus_token.hpp"
#include "tokens/private_token.hpp"
#include "tokens/protected_token.hpp"
#include "tokens/public_token.hpp"
#include "tokens/question_dot_token.hpp"
#include "tokens/question_token.hpp"
#include "tokens/readonly_token.hpp"
#include "tokens/require_token.hpp"
#include "tokens/return_token.hpp"
#include "tokens/satisfies_token.hpp"
#include "tokens/semicolon_token.hpp"
#include "tokens/set_token.hpp"
#include "tokens/shebang_token.hpp"
#include "tokens/slash_eq_token.hpp"
#include "tokens/slash_token.hpp"
#include "tokens/static_token.hpp"
#include "tokens/string_token.hpp"
#include "tokens/super_token.hpp"
#include "tokens/switch_token.hpp"
#include "tokens/symbol_token.hpp"
#include "tokens/template_end_token.hpp"
#include "tokens/template_start_token.hpp"
#include "tokens/this_token.hpp"
#include "tokens/throw_token.hpp"
#include "tokens/tilde_token.hpp"
#include "tokens/triple_dot_token.hpp"
#include "tokens/triple_eq_token.hpp"
#include "tokens/triple_greater_eq_token.hpp"
#include "tokens/triple_greater_token.hpp"
#include "tokens/true_token.hpp"
#include "tokens/try_token.hpp"
#include "tokens/type_token.hpp"
#include "tokens/typeof_token.hpp"
#include "tokens/undefined_token.hpp"
#include "tokens/unique_token.hpp"
#include "tokens/unknown_token.hpp"
#include "tokens/var_token.hpp"
#include "tokens/void_token.hpp"
#include "tokens/while_token.hpp"
#include "tokens/with_token.hpp"
#include "tokens/yield_token.hpp"

#include "detail/all_tokens.ipp"

namespace tscc::lex {

/**
 * \brief An exception thrown when trying to extract the basic_token from an undefined token
 */
class token_undefined : public std::exception {
public:
	token_undefined() noexcept = default;

	const char* what() const noexcept override;
};

/**
 * \brief A container for any of the possible basic_tokens
 */
class token {
public:
	/**
	 * \brief Get a reference to the underlying basic_token
	 */
	const tokens::basic_token& operator*() const noexcept;

	/**
	 * \brief Get a reference to the underlying basic_token
	 */
	tokens::basic_token& operator*() noexcept;

	/**
	 * \brief Get a pointer to the underlying basic_token
	 */
	const tokens::basic_token* operator->() const noexcept;

	/**
	 * \brief Get a pointer to the underlying basic_token
	 */
	tokens::basic_token* operator->() noexcept;

	/**
	 * \brief Visit the token by its specific type
	 *
	 * The visitor must provide a matching overload for every type
	 * of token available in the \see tsc::lex::tokens namespace.
	 * This can either be done explicitly or by providing an overload
	 * for the generic base basic_token.
	 *
	 * For example:
	 * \example
	 * \code c++
	 * struct comment_token_visitor
	 * {
	 *   int operator()(const tsc::lex::tokens::comment_token& token) const {
	 *     ...
	 *   }
	 *
	 *   int operator()(const tsc::lex::tokens::basic_token&) const {
	 *     return 0;
	 *   }
	 * };
	 * \endcode
	 */
	template <typename T>
	decltype(auto) visit(T&& visitor) const {
		if (!token_)
			throw token_undefined();
		return std::visit(std::forward<T>(visitor), *token_);
	}

	/**
	 * \brief Get the location where the token is defined
	 */
	const source_location& location() const noexcept;

	/**
	 * \brief Emplace a new value into the token
	 */
	template <typename Token, typename... Args>
	void emplace_token(const source_location& location, Args&&... args) {
		if (token_) {
			token_->emplace<Token>(std::forward<Args>(args)...);
		} else {
			token_.emplace(all_tokens_t{Token{std::forward<Args>(args)...}});
		}
		location_ = location;
	}

	/**
	 * \brief Get whether the token is of the particular type
	 */
	template <typename Token>
	constexpr std::enable_if_t<std::is_base_of_v<tokens::basic_token, Token>,
							   bool>
	is() const noexcept {
		if (!token_)
			return false;
		return std::holds_alternative<Token>(*token_);
	}

	/**
	 * \brief Set the token to an undefined value
	 */
	void undefine();

	/**
	 * \brief Compare the token for equality vs another token
	 */
	bool operator==(const token& other) const;

	/**
	 * \brief Compare the token for inequality vs another token
	 */
	bool operator!=(const token& other) const;

private:
	using all_tokens_t = std::variant<ALL_TOKEN_TYPES>;

	source_location location_;
	std::optional<all_tokens_t> token_;
};

}  // namespace tscc::lex

#undef ALL_TOKEN_TYPES