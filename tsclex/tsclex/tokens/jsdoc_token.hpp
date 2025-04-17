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

#include <span>
#include <string>
#include <variant>
#include <vector>
#include "basic_token.hpp"

namespace tscc::lex::tokens {

/**
 * @brief A typescript token that represents a comment
 */
class jsdoc_token : public basic_token {
public:
	enum class jsdoc_part_type { string_value, tag, type_parameter };

	/**
	 * \brief The base class for a jsdoc "token"
	 */
	class jsdoc_part {
	public:
		virtual ~jsdoc_part() = default;

		virtual jsdoc_part_type type() const = 0;

		virtual bool equivalent(const jsdoc_part& other) const = 0;

		virtual std::ostream& write(std::ostream& to) const = 0;
	};

	/**
	 * \brief A string literal
	 */
	class jsdoc_string_part : public jsdoc_part {
		std::u32string str_;

	public:
		jsdoc_string_part(std::u32string str) : str_(std::move(str)) {}

		const std::u32string& str() const { return str_; }

		jsdoc_part_type type() const override;

		bool equivalent(const jsdoc_part& other) const override;

		std::ostream& write(std::ostream& to) const override;
	};

	/**
	 * \brief A JSDoc Tag like @@returns
	 */
	class jsdoc_tag_part : public jsdoc_part {
		std::string tag_;

	public:
		jsdoc_tag_part(std::string tag) : tag_(std::move(tag)) {}

		const std::string& tag() const { return tag_; }

		jsdoc_part_type type() const override;

		bool equivalent(const jsdoc_part& other) const override;

		std::ostream& write(std::ostream& to) const override;
	};

	/**
	 * \brief A {type} label for a JSDoc tag
	 */
	class jsdoc_type_part : public jsdoc_part {
		std::u32string type_;

	public:
		jsdoc_type_part(std::u32string type) : type_(std::move(type)) {}

		const std::u32string& type_name() const { return type_; }

		jsdoc_part_type type() const override;

		bool equivalent(const jsdoc_part& other) const override;

		std::ostream& write(std::ostream& to) const override;
	};

	/**
	 * \brief a JSDoc line which is just a collection of parts
	 */
	class jsdoc_line {
		using entry =
			std::variant<jsdoc_string_part, jsdoc_tag_part, jsdoc_type_part>;
		std::vector<entry> entries_;

	public:
		std::size_t size() const { return entries_.size(); }

		bool empty() const { return entries_.empty(); }

		const jsdoc_part& operator[](std::size_t index) const;

		bool equivalent(const jsdoc_line& other) const;

		template <typename T>
		void append(T&& entry) {
			entries_.emplace_back(std::forward<T>(entry));
		}

		std::ostream& write(std::ostream& to) const;
	};

	jsdoc_token(const std::span<std::u32string>& comment_lines);

	bool operator==(const jsdoc_token& other) const;
	bool operator!=(const jsdoc_token& other) const;

	std::span<jsdoc_line> lines() const;

	std::string to_string() const override;

private:
	std::vector<jsdoc_line> lines_;
};

}  // namespace tscc::lex::tokens