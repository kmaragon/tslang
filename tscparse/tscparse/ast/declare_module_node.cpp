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

#include "declare_module_node.hpp"

using namespace tscc;
using namespace tscc::parse::ast;

declare_module_node::declare_module_node(lex::token declare_keyword,
										 lex::token module_keyword)
	: declare_keyword_(std::move(declare_keyword)),
	  module_keyword_(std::move(module_keyword)) {}

const lex::token* declare_module_node::declare_keyword() const noexcept {
	return &declare_keyword_;
}

const lex::token* declare_module_node::module_keyword() const noexcept {
	return &module_keyword_;
}

lexeme<std::string> declare_module_node::module_name() const {
	return {&module_name_};
}
