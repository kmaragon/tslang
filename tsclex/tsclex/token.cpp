/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2025. Keef Aragon
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

#include "token.hpp"

using namespace tscc::lex;

namespace {

struct basic_token_extractor {
	tokens::basic_token* operator()(tokens::basic_token& token) const {
		return &token;
	}

	const tokens::basic_token* operator()(
		const tokens::basic_token& token) const {
		return &token;
	}
};

}  // namespace

tokens::basic_token& token::operator*() noexcept {
	basic_token_extractor ex;
	return *std::visit(ex, *token_);
}

const tokens::basic_token& token::operator*() const noexcept {
	basic_token_extractor ex;
	return *std::visit(ex, *token_);
}

tokens::basic_token* token::operator->() noexcept {
	if (!token_)
		return nullptr;

	basic_token_extractor ex;
	return std::visit(ex, *token_);
}

const tokens::basic_token* token::operator->() const noexcept {
	if (!token_)
		return nullptr;

	basic_token_extractor ex;
	return std::visit(ex, *token_);
}

const source_location& token::location() const noexcept {
	return location_;
}

void token::undefine() {
	token_.reset();
}

bool token::operator==(const tscc::lex::token& other) const {
	if (!token_)
		return !other.token_;

	if (!other.token_)
		return false;

	return (*token_) == (*other.token_);
}

bool token::operator!=(const tscc::lex::token& other) const {
	return !operator==(other);
}