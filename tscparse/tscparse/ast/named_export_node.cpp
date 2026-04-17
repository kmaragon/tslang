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

#include "named_export_node.hpp"

using namespace tscc::parse::ast;

ast_node::kind named_export_node::node_kind() const noexcept {
	return kind::named_export;
}

const std::vector<named_specifier>& named_export_node::specifiers()
	const noexcept {
	return specifiers_;
}

lexeme<std::string> named_export_node::module_specifier() const {
	if (!module_specifier_)
		return {};
	return {&module_specifier_};
}
