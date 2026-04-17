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

#include <cstdint>
#include <memory>

namespace tscc::parse::ast {

/**
 * \brief Base class for all AST nodes
 *
 * Provides parent pointer for tree navigation and facilities for
 * managing child node ownership.
 */
class ast_node {
	static constexpr uint16_t exportable_flag = 0x400;

	const ast_node* parent_ = nullptr;

public:
	/**
	 * \brief Top-level discriminant for AST node dispatch
	 *
	 * Upper bits encode trait flags (e.g. exportable). Use the
	 * static helpers like is_exportable() to test traits.
	 * For type expression nodes, use type_node::type_kind() for
	 * further discrimination.
	 */
	enum class kind : uint16_t {
		source_file     = 0,
		type_parameter  = 1,
		type_definition = 2,
		import_kind     = exportable_flag | 3,
		namespace_kind  = exportable_flag | 4,
		declare_module  = exportable_flag | 5,
		type              = exportable_flag | 6,
		export_assignment = exportable_flag | 7,
		export_star       = exportable_flag | 8,
		named_export      = exportable_flag | 9,
	};

	/**
	 * \brief Test whether a node kind represents an exportable declaration
	 */
	[[nodiscard]] static constexpr bool is_exportable(kind k) noexcept {
		return (static_cast<uint16_t>(k) & exportable_flag) != 0;
	}

	virtual ~ast_node() = default;

	/**
	 * \brief Get the top-level node kind
	 */
	[[nodiscard]] virtual kind node_kind() const noexcept = 0;

	/**
	 * \brief Get the parent node
	 */
	const ast_node* parent() const noexcept { return parent_; }

protected:
	/**
	 * \brief Set the parent node
	 */
	void set_parent(const ast_node* p) noexcept { parent_ = p; }

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
};

}  // namespace tscc::parse::ast