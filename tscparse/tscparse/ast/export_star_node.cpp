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

#include "export_star_node.hpp"

using namespace tscc::parse::ast;

export_star_node::export_star_node(lex::token asterisk_token)
	: asterisk_(std::move(asterisk_token)) {}

ast_node::kind export_star_node::node_kind() const noexcept {
	return kind::export_star;
}

const tscc::lex::token* export_star_node::asterisk() const noexcept {
	return &asterisk_;
}

lexeme<std::string_view> export_star_node::namespace_name() const {
	if (!as_name_)
		return {};
	return {&as_name_};
}

lexeme<std::string> export_star_node::module_specifier() const {
	return {&module_specifier_};
}
