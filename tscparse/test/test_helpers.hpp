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

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <sstream>
#include <tscfakes/fake_source.hpp>
#include <tsclex/lexer.hpp>
#include <tscparse/parser.hpp>

namespace test_utils {

/**
 * \brief Holds all parsing artifacts to keep them alive during testing
 *
 * Members are heap-allocated so that moving the struct does not
 * invalidate the references the lexer/parser hold into the stream.
 */
struct parse_result {
	std::unique_ptr<std::stringstream> stream;
	std::shared_ptr<fake_source> source;
	std::unique_ptr<tscc::lex::lexer> lexer;
	std::unique_ptr<tscc::parse::parser> parser;
	std::unique_ptr<tscc::parse::ast::ast_node> node;
};

/**
 * \brief Parse a source string and return the first top-level AST node
 */
inline parse_result parse_first_node(const std::string& input) {
	parse_result r;
	r.stream = std::make_unique<std::stringstream>(input);
	r.source = std::make_shared<fake_source>("test.ts");
	r.lexer = std::make_unique<tscc::lex::lexer>(*r.stream, r.source);
	r.parser = std::make_unique<tscc::parse::parser>(*r.lexer);

	auto it = r.parser->begin();
	if (it != r.parser->end()) {
		r.node = std::move(*it);
	}
	return r;
}

/**
 * \brief Parse and return the first node cast to a specific type
 *
 * Fails the test if the result is null or not the expected node type.
 * Results are kept alive in a static vector for the lifetime of the test run.
 */
template <typename Node>
const Node& parse_node(const std::string& input) {
	static std::vector<parse_result> results;
	results.emplace_back(parse_first_node(input));
	auto& r = results.back();
	REQUIRE(r.node != nullptr);
	auto* typed = dynamic_cast<const Node*>(r.node.get());
	REQUIRE(typed != nullptr);
	return *typed;
}

}  // namespace test_utils
