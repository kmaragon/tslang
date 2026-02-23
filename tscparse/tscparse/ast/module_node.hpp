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
#include <vector>
#include "ast_node.hpp"

namespace tscc::parse::state {
class module_scope_state;
class namespace_state;
}

namespace tscc::parse::ast {

/**
 * \brief Base class for AST nodes that represent module scopes
 *
 * Owns a children vector of top-level declarations. Both source_file_node
 * (root) and future declare_module_node inherit from this to share the
 * container and accessor.
 */
class module_node : public ast_node {
public:
	/**
	 * \brief Get the declarations in this module scope
	 */
	[[nodiscard]] const std::vector<std::unique_ptr<const ast_node>>& children()
		const noexcept;

protected:
	module_node() = default;

	std::vector<std::unique_ptr<const ast_node>> children_;

private:
	friend class ::tscc::parse::state::module_scope_state;
	friend class ::tscc::parse::state::namespace_state;
};

}  // namespace tscc::parse::ast
