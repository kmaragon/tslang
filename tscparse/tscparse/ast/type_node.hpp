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

#include "ast_node.hpp"

namespace tscc::parse::ast {

/**
 * \brief Abstract base class for all type expression nodes
 *
 * Provides compile-time safety for type positions: any AST slot that
 * expects a type expression uses unique_ptr<const type_node> rather than
 * bare ast_node, catching misuse at compile time.
 */
class type_node : public ast_node {
public:
	~type_node() override = default;

protected:
	type_node() = default;
};

}  // namespace tscc::parse::ast
