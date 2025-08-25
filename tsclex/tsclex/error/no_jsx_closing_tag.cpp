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

#include "no_jsx_closing_tag.hpp"

using namespace tscc::lex;

no_jsx_closing_tag::no_jsx_closing_tag(
	const tscc::lex::source_location& location, const std::string& element)
	: lex_error(location) {
	constexpr std::string_view templ = "JSX element '{}' has no corresponding closing tag";
	msg_.reserve(templ.size() + element.size() - 2);
	msg_.append(templ.substr(0, templ.find('{')));
	msg_.append(element);
	msg_.append(templ.substr(templ.find('}') + 1));
}

const char* no_jsx_closing_tag::what() const noexcept {
	return msg_.c_str();
}

error_code no_jsx_closing_tag::code() const noexcept {
	return error_code::ts17008;
}
