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

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <tsclex/lexer.hpp>
#include <tsclex/token.hpp>
#include "fake_source.hpp"

#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

namespace test_utils {
	inline auto create_test_setup() {
		auto file = std::make_shared<std::stringstream>();
		auto source = std::make_shared<fake_source>(__FILE__);

		auto create_lexer = [file, source](
								const std::string& input,
								tscc::lex::language_version version =
									tscc::lex::language_version::es_next) {
			file->str("");
			file->clear();
			*file << input;
			file->flush();
			return tscc::lex::lexer(*file, source, version);
		};

		auto tokenize = [create_lexer](const std::string& input) {
			auto lexer = create_lexer(input);
			return std::vector<tscc::lex::token>{lexer.begin(), lexer.end()};
		};

		return std::make_tuple(file, source, create_lexer, tokenize);
	}
}