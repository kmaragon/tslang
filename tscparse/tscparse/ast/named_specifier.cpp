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

#include "named_specifier.hpp"
#include <tsclex/tokens/default_token.hpp>

using namespace tscc::parse::ast;

lexeme<std::string_view> named_specifier::name() const {
	if (name_.is<lex::tokens::default_token>())
		return {};
	return {&name_};
}

bool named_specifier::is_default() const noexcept {
	return name_.is<lex::tokens::default_token>();
}

const tscc::lex::token* named_specifier::type_keyword() const noexcept {
	return type_keyword_ ? &type_keyword_ : nullptr;
}

lexeme<std::string_view> named_specifier::alias() const {
	return alias_ ? lexeme<std::string_view>{&alias_}
				  : lexeme<std::string_view>{};
}
