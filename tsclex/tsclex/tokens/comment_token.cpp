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

#include "comment_token.hpp"

using namespace tscc::lex::tokens;

comment_token::comment_token(std::string comment_body)
	: body_(std::move(comment_body)) {}

bool comment_token::operator==(
	const tscc::lex::tokens::comment_token& other) const {
	return body_ == other.body_;
}

bool comment_token::operator!=(
	const tscc::lex::tokens::comment_token& other) const {
	return body_ != other.body_;
}

std::string comment_token::to_string() const
{
	return body_;
}