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

#include <functional>
#include <memory>
#include <tsclex/source_location.hpp>

namespace tscc::parse::ast {

/**
 * \brief Base class for all AST nodes
 *
 * Provides parent pointer for tree navigation and facilities for
 * managing child node ownership.
 */
class ast_node {
protected:
	ast_node* parent_ = nullptr;

public:
	virtual ~ast_node() = default;

	/**
	 * \brief Get the parent node
	 */
	ast_node* parent() const noexcept { return parent_; }

	/**
	 * \brief Set the parent node
	 */
	void set_parent(ast_node* p) noexcept { parent_ = p; }

	/**
	 * \brief Adopt a child node, setting its parent pointer
	 */
	template <typename T>
	std::unique_ptr<T> adopt_child(std::unique_ptr<T> child) {
		if (child) {
			child->set_parent(this);
		}
		return child;
	}

	/**
	 * \brief Get the source location of this node
	 */
	virtual const lex::source_location& location() const = 0;

	/**
	 * \brief Visit all direct children of this node
	 *
	 * This enables generic tree traversal without knowing the
	 * specific node type.
	 */
	virtual void visit_children(
		std::function<void(const ast_node*)> visitor) const = 0;
};

}  // namespace tscc::parse::ast