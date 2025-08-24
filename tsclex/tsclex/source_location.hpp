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

#include <memory>
#include "source.hpp"

namespace tscc::lex {

/**
 * \brief An encapsulation of a particular spot in some source code
 */
class source_location {
public:
	source_location() = default;

	source_location(std::shared_ptr<source> source,
					std::size_t line,
					std::size_t column,
					std::size_t offset) noexcept;
	source_location(const source_location&) noexcept = default;

	/**
	 * \brief Get the line number for the location
	 */
	std::size_t line() const noexcept;

	/**
	 * \brief Get the column number for the location
	 */
	std::size_t column() const noexcept;

	/**
	 * \brief Get the offset for the location
	 */
	std::size_t offset() const noexcept;

	/**
	 * \brief Get a source location on the same line but
	 */
	source_location operator+(std::size_t offset) const noexcept;

private:
	std::shared_ptr<source> source_;
	std::size_t line_;
	std::size_t column_;
	std::size_t offset_;
};

}  // namespace tscc::lex
