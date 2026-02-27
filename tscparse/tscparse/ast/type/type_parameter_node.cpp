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

#include "type_parameter_node.hpp"

using namespace tscc::parse::ast;

type_parameter_node::type_parameter_node(tscc::lex::token name)
	: name_(std::move(name)) {}

lexeme<std::string_view> type_parameter_node::name() const {
	return {&name_};
}

bool type_parameter_node::has_in() const noexcept {
	return static_cast<bool>(in_keyword_);
}

bool type_parameter_node::has_out() const noexcept {
	return static_cast<bool>(out_keyword_);
}

const type_definition* type_parameter_node::constraint() const noexcept {
	return constraint_.get();
}

const type_definition* type_parameter_node::default_type() const noexcept {
	return default_.get();
}
