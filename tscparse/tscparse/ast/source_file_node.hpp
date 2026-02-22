/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2026. Keef Aragon
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
#include <tsclex/source.hpp>
#include "module_node.hpp"

namespace tscc::parse {
class parser;
}

namespace tscc::parse::ast {

/**
 * \brief Root AST node representing a translation unit (source file)
 *
 * Owns the top-level declarations parsed from a single file.
 * Only the parser can construct instances and add children.
 */
class source_file_node final : public module_node {
	friend class ::tscc::parse::parser;

public:
	/**
	 * \brief Get the source file for this translation unit
	 */
	[[nodiscard]] const std::shared_ptr<lex::source>& source() const noexcept;

private:
	explicit source_file_node(std::shared_ptr<lex::source> source);

	std::shared_ptr<lex::source> source_;
};

}  // namespace tscc::parse::ast
