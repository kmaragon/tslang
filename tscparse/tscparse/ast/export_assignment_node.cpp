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

#include "export_assignment_node.hpp"

using namespace tscc::parse::ast;

export_assignment_node::export_assignment_node(lex::token equals_token)
	: equals_(std::move(equals_token)) {}

ast_node::kind export_assignment_node::node_kind() const noexcept {
	return kind::export_assignment;
}

const tscc::lex::token* export_assignment_node::equals_token() const noexcept {
	return &equals_;
}

lexeme<std::string_view> export_assignment_node::identifier() const {
	return {&identifier_};
}
