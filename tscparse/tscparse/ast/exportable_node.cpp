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

#include "exportable_node.hpp"

using namespace tscc::parse::ast;

const tscc::lex::token* exportable_node::export_keyword() const noexcept {
	return export_keyword_ ? &export_keyword_ : nullptr;
}

void exportable_node::set_export_keyword(lex::token token) noexcept {
	export_keyword_ = std::move(token);
}
