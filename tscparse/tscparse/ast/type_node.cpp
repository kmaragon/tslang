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

#include "type_node.hpp"

using namespace tscc::parse::ast;

type_node::type_node(lex::token type_keyword)
	: type_keyword_(std::move(type_keyword)) {}

const tscc::lex::token* type_node::export_keyword() const noexcept {
	return export_keyword_ ? &export_keyword_ : nullptr;
}

const tscc::lex::token* type_node::declare_keyword() const noexcept {
	return declare_keyword_ ? &declare_keyword_ : nullptr;
}

const tscc::lex::token* type_node::keyword() const noexcept {
	return &type_keyword_;
}

lexeme<std::string_view> type_node::name() const {
	return {&name_};
}

const std::vector<std::unique_ptr<const type_parameter_node>>&
type_node::type_parameters() const noexcept {
	return type_parameters_;
}

const type_definition& type_node::type() const noexcept {
	return *type_;
}

bool type_node::ambient() const noexcept {
	return declare_keyword_;
}
