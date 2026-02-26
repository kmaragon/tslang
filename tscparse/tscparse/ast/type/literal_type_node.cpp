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

#include "literal_type_node.hpp"

using namespace tscc::parse::ast;

literal_type_node::literal_type_node(tscc::lex::token value)
	: value_(std::move(value)) {}

literal_type_node::literal_type_node(tscc::lex::token minus,
									 tscc::lex::token value)
	: minus_(std::move(minus)), value_(std::move(value)) {}

const tscc::lex::token* literal_type_node::minus_token() const noexcept {
	return minus_ ? &minus_ : nullptr;
}

const tscc::lex::token& literal_type_node::value_token() const noexcept {
	return value_;
}
