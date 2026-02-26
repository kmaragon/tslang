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

#include "array_type_node.hpp"

using namespace tscc::parse::ast;

array_type_node::array_type_node(std::unique_ptr<const type_node> element,
								 tscc::lex::token open_bracket,
								 tscc::lex::token close_bracket)
	: element_(std::move(element)),
	  open_bracket_(std::move(open_bracket)),
	  close_bracket_(std::move(close_bracket)) {}

const type_node& array_type_node::element_type() const noexcept {
	return *element_;
}
