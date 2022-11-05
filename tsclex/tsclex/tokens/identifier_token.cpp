/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2022. Keef Aragon
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

#include "identifier_token.hpp"

using namespace tscc::lex::tokens;

identifier_token::identifier_token(std::wstring identifier)
	: id_(std::move(identifier)) {}

bool identifier_token::operator==(
	const tscc::lex::tokens::identifier_token& other) const {
	return id_ == other.id_;
}

bool identifier_token::operator!=(
	const tscc::lex::tokens::identifier_token& other) const {
	return id_ != other.id_;
}

std::wstring identifier_token::to_string() const {
	return id_;
}