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

#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <tscfakes/fake_source.hpp>
#include <tsclex/lexer.hpp>
#include <tsclex/tokens/comment_token.hpp>
#include <tsclex/tokens/jsdoc_token.hpp>
#include <tsclex/tokens/multiline_comment_token.hpp>
#include <tsclex/tokens/newline_token.hpp>
#include <tscparse/error/declaration_or_statement_expected.hpp>
#include <tscparse/error/expected_token.hpp>
#include <tscparse/filtered_token_index.hpp>
#include <tscparse/parser.hpp>

namespace {

// Test helper: filter to only regular comments (excluding JSDoc)
std::vector<tscc::parse::trivia_ref> filter_comments(
	const std::vector<tscc::parse::trivia_ref>& refs) {
	std::vector<tscc::parse::trivia_ref> result;
	for (const auto& ref : refs) {
		if (ref.token.is<tscc::lex::tokens::comment_token>() ||
			ref.token.is<tscc::lex::tokens::multiline_comment_token>()) {
			result.push_back(ref);
		}
	}
	return result;
}

// Test helper: filter to only JSDoc comments
std::vector<tscc::parse::trivia_ref> filter_jsdoc(
	const std::vector<tscc::parse::trivia_ref>& refs) {
	std::vector<tscc::parse::trivia_ref> result;
	for (const auto& ref : refs) {
		if (ref.token.is<tscc::lex::tokens::jsdoc_token>()) {
			result.push_back(ref);
		}
	}
	return result;
}

}  // namespace

TEST_CASE("Parser Basic Construction", "[parser]") {
	SECTION("Parser can be constructed") {
		std::stringstream input("");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		tscc::parse::parser parser(lexer);

		REQUIRE(parser.begin() == parser.end());
	}

	SECTION("Parser with trivia index") {
		std::stringstream input("");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		tscc::parse::trivia_index trivia;
		tscc::parse::parser parser(lexer, &trivia);

		REQUIRE(parser.begin() == parser.end());
	}
}

TEST_CASE("Parser Iterator", "[parser]") {
	SECTION("Empty input produces empty iterator range") {
		std::stringstream input("");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);
		tscc::parse::parser parser(lexer);

		std::vector<std::unique_ptr<tscc::parse::ast::ast_node>> nodes;
		for (auto& node : parser) {
			nodes.push_back(std::move(node));
		}

		REQUIRE(nodes.empty());
	}
}

TEST_CASE("Trivia Index", "[trivia]") {
	SECTION("Empty index") {
		tscc::parse::trivia_index index;
		index.finalize();

		REQUIRE(index.all().empty());
	}

	SECTION("Filter comments from empty index") {
		tscc::parse::trivia_index index;
		index.finalize();

		auto comments = filter_comments(index.all());
		REQUIRE(comments.empty());
	}

	SECTION("Filter JSDoc from empty index") {
		tscc::parse::trivia_index index;
		index.finalize();

		auto jsdocs = filter_jsdoc(index.all());
		REQUIRE(jsdocs.empty());
	}

	SECTION("Add trivia tokens and retrieve all") {
		std::stringstream input("// comment\n/** jsdoc */\n");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		std::vector<tscc::lex::token> tokens{lexer.begin(), lexer.end()};

		tscc::parse::trivia_index index;
		// Filter only comment tokens (not newlines)
		for (const auto& tok : tokens) {
			if (tok.is<tscc::lex::tokens::comment_token>() ||
				tok.is<tscc::lex::tokens::multiline_comment_token>() ||
				tok.is<tscc::lex::tokens::jsdoc_token>()) {
				index.emplace(tok, nullptr,
							  tscc::parse::trivia_ref::relationship::orphaned);
			}
		}
		index.finalize();

		auto all = index.all();
		REQUIRE(all.size() == 2);  // comment and jsdoc
	}

	SECTION("Filter comments excludes JSDoc") {
		std::stringstream input("// comment\n/* block */\n/** jsdoc */\n");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		std::vector<tscc::lex::token> tokens{lexer.begin(), lexer.end()};

		tscc::parse::trivia_index index;
		for (const auto& tok : tokens) {
			if (tok.is<tscc::lex::tokens::comment_token>() ||
				tok.is<tscc::lex::tokens::multiline_comment_token>() ||
				tok.is<tscc::lex::tokens::jsdoc_token>()) {
				index.emplace(tok, nullptr,
							  tscc::parse::trivia_ref::relationship::orphaned);
			}
		}
		index.finalize();

		auto comments = filter_comments(index.all());
		REQUIRE(comments.size() == 2);	// single-line and block, not jsdoc
	}

	SECTION("Filter JSDoc excludes regular comments") {
		std::stringstream input("// comment\n/** jsdoc1 */\n/** jsdoc2 */\n");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		std::vector<tscc::lex::token> tokens{lexer.begin(), lexer.end()};

		tscc::parse::trivia_index index;
		for (const auto& tok : tokens) {
			if (tok.is<tscc::lex::tokens::comment_token>() ||
				tok.is<tscc::lex::tokens::multiline_comment_token>() ||
				tok.is<tscc::lex::tokens::jsdoc_token>()) {
				index.emplace(tok, nullptr,
							  tscc::parse::trivia_ref::relationship::orphaned);
			}
		}
		index.finalize();

		auto jsdocs = filter_jsdoc(index.all());
		REQUIRE(jsdocs.size() == 2);
	}

	SECTION("Throws when adding non-trivia token") {
		std::stringstream input("const x = 42;");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		std::vector<tscc::lex::token> tokens{lexer.begin(), lexer.end()};
		REQUIRE(tokens.size() > 0);

		tscc::parse::trivia_index index;
		// Should throw when trying to add a real token (like const, identifier,
		// etc.)
		REQUIRE_THROWS_AS(
			index.emplace(tokens[0], nullptr,
						  tscc::parse::trivia_ref::relationship::orphaned),
			std::invalid_argument);
	}

	SECTION("Find trivia at specific location") {
		std::stringstream input("// comment\n/** jsdoc */\n");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		std::vector<tscc::lex::token> tokens{lexer.begin(), lexer.end()};

		tscc::parse::trivia_index index;
		std::vector<tscc::lex::token> comment_tokens;
		for (const auto& tok : tokens) {
			if (tok.is<tscc::lex::tokens::comment_token>() ||
				tok.is<tscc::lex::tokens::multiline_comment_token>() ||
				tok.is<tscc::lex::tokens::jsdoc_token>()) {
				index.emplace(tok, nullptr,
							  tscc::parse::trivia_ref::relationship::orphaned);
				comment_tokens.push_back(tok);
			}
		}
		index.finalize();

		REQUIRE(comment_tokens.size() >= 1);
		// Find at location of first comment
		auto found = index.find_at(comment_tokens[0].location());
		REQUIRE(found.has_value());
		const auto& found_loc = found->token.location();
		const auto& expected_loc = comment_tokens[0].location();
		REQUIRE(found_loc.line() == expected_loc.line());
		REQUIRE(found_loc.column() == expected_loc.column());
	}

	SECTION("Find trivia in range") {
		std::stringstream input("// 1\n\n// 2\n\n\n// 3\n\n\n\n// 4\n");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		std::vector<tscc::lex::token> tokens{lexer.begin(), lexer.end()};

		tscc::parse::trivia_index index;
		for (const auto& tok : tokens) {
			if (tok.is<tscc::lex::tokens::comment_token>() ||
				tok.is<tscc::lex::tokens::multiline_comment_token>() ||
				tok.is<tscc::lex::tokens::jsdoc_token>()) {
				index.emplace(tok, nullptr,
							  tscc::parse::trivia_ref::relationship::orphaned);
			}
		}
		index.finalize();

		// Find tokens between line 2 and line 7
		tscc::lex::source_location start(source, 2, 1, 0);
		tscc::lex::source_location end(source, 7, 1, 100);
		auto found = index.find_in_range(start, end);

		// Should include comments in that range
		REQUIRE(found.size() > 0);
	}

	SECTION("Trivia sorted after finalize") {
		std::stringstream input("// 1\n// 2\n// 3\n");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		std::vector<tscc::lex::token> tokens{lexer.begin(), lexer.end()};

		tscc::parse::trivia_index index;
		std::vector<tscc::lex::token> comment_tokens;
		for (const auto& tok : tokens) {
			if (tok.is<tscc::lex::tokens::comment_token>() ||
				tok.is<tscc::lex::tokens::multiline_comment_token>() ||
				tok.is<tscc::lex::tokens::jsdoc_token>()) {
				comment_tokens.push_back(tok);
			}
		}

		// Add in reverse order
		for (auto it = comment_tokens.rbegin(); it != comment_tokens.rend();
			 ++it) {
			index.emplace(*it, nullptr,
						  tscc::parse::trivia_ref::relationship::orphaned);
		}
		index.finalize();

		auto all = index.all();
		REQUIRE(all.size() == comment_tokens.size());
		REQUIRE(all.size() > 0);
		// Should be sorted by location
		for (size_t i = 1; i < all.size(); ++i) {
			const auto& prev_loc = all[i - 1].token.location();
			const auto& curr_loc = all[i].token.location();
			REQUIRE(prev_loc.line() <= curr_loc.line());
			if (prev_loc.line() == curr_loc.line()) {
				REQUIRE(prev_loc.column() < curr_loc.column());
			}
		}
	}

	SECTION("find_before returns nearest trivia before location") {
		std::stringstream input("// first\n// second\n// third\n");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		std::vector<tscc::lex::token> tokens{lexer.begin(), lexer.end()};

		tscc::parse::trivia_index index;
		std::vector<tscc::lex::token> comment_tokens;
		for (const auto& tok : tokens) {
			if (tok.is<tscc::lex::tokens::comment_token>() ||
				tok.is<tscc::lex::tokens::multiline_comment_token>() ||
				tok.is<tscc::lex::tokens::jsdoc_token>()) {
				index.emplace(tok, nullptr,
							  tscc::parse::trivia_ref::relationship::orphaned);
				comment_tokens.push_back(tok);
			}
		}
		index.finalize();

		REQUIRE(comment_tokens.size() == 3);

		// Query from location after second comment - should find "// second"
		auto after_second = comment_tokens[1].location().line() + 1;
		tscc::lex::source_location query(source, after_second, 0, 0);
		auto found = index.find_before(query);
		REQUIRE(found.has_value());
		REQUIRE(found->token->to_string() == "// second");
	}

	SECTION("find_before returns nullopt when no trivia before") {
		std::stringstream input("// line3\n// line5\n");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		std::vector<tscc::lex::token> tokens{lexer.begin(), lexer.end()};

		tscc::parse::trivia_index index;
		std::vector<tscc::lex::token> comment_tokens;
		for (const auto& tok : tokens) {
			if (tok.is<tscc::lex::tokens::comment_token>() ||
				tok.is<tscc::lex::tokens::multiline_comment_token>() ||
				tok.is<tscc::lex::tokens::jsdoc_token>()) {
				index.emplace(tok, nullptr,
							  tscc::parse::trivia_ref::relationship::orphaned);
				comment_tokens.push_back(tok);
			}
		}
		index.finalize();

		REQUIRE(comment_tokens.size() >= 1);
		// Query from exactly the first comment's location - nothing strictly
		// before it
		auto first_line = comment_tokens[0].location().line();
		auto first_col = comment_tokens[0].location().column();
		tscc::lex::source_location query(source, first_line, first_col, 0);
		auto found = index.find_before(query);
		REQUIRE_FALSE(found.has_value());
	}

	SECTION("find_before with empty index returns nullopt") {
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::parse::trivia_index index;
		index.finalize();

		tscc::lex::source_location query(source, 5, 1, 0);
		auto found = index.find_before(query);
		REQUIRE_FALSE(found.has_value());
	}

	SECTION("find_after returns nearest trivia after location") {
		std::stringstream input("// first\n// second\n// third\n");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		std::vector<tscc::lex::token> tokens{lexer.begin(), lexer.end()};

		tscc::parse::trivia_index index;
		std::vector<tscc::lex::token> comment_tokens;
		for (const auto& tok : tokens) {
			if (tok.is<tscc::lex::tokens::comment_token>() ||
				tok.is<tscc::lex::tokens::multiline_comment_token>() ||
				tok.is<tscc::lex::tokens::jsdoc_token>()) {
				index.emplace(tok, nullptr,
							  tscc::parse::trivia_ref::relationship::orphaned);
				comment_tokens.push_back(tok);
			}
		}
		index.finalize();

		REQUIRE(comment_tokens.size() == 3);

		// Query from location before second comment - should find "// second"
		auto second_line = comment_tokens[1].location().line();
		tscc::lex::source_location query(source, second_line - 1, 99, 0);
		auto found = index.find_after(query);
		REQUIRE(found.has_value());
		REQUIRE(found->token->to_string() == "// second");
	}

	SECTION("find_after returns nullopt when no trivia after") {
		std::stringstream input("// line1\n// line2\n");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		std::vector<tscc::lex::token> tokens{lexer.begin(), lexer.end()};

		tscc::parse::trivia_index index;
		for (const auto& tok : tokens) {
			if (tok.is<tscc::lex::tokens::comment_token>() ||
				tok.is<tscc::lex::tokens::multiline_comment_token>() ||
				tok.is<tscc::lex::tokens::jsdoc_token>()) {
				index.emplace(tok, nullptr,
							  tscc::parse::trivia_ref::relationship::orphaned);
			}
		}
		index.finalize();

		// Query from line 10 - nothing after
		tscc::lex::source_location query(source, 10, 1, 0);
		auto found = index.find_after(query);
		REQUIRE_FALSE(found.has_value());
	}

	SECTION("find_after with empty index returns nullopt") {
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::parse::trivia_index index;
		index.finalize();

		tscc::lex::source_location query(source, 1, 1, 0);
		auto found = index.find_after(query);
		REQUIRE_FALSE(found.has_value());
	}

	SECTION("find_before and find_after with same-line trivia") {
		// Multiple comments on the same line
		std::stringstream input("/* a */ /* b */ /* c */\n");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);

		std::vector<tscc::lex::token> tokens{lexer.begin(), lexer.end()};

		tscc::parse::trivia_index index;
		std::vector<tscc::lex::token> comment_tokens;
		for (const auto& tok : tokens) {
			if (tok.is<tscc::lex::tokens::comment_token>() ||
				tok.is<tscc::lex::tokens::multiline_comment_token>() ||
				tok.is<tscc::lex::tokens::jsdoc_token>()) {
				index.emplace(tok, nullptr,
							  tscc::parse::trivia_ref::relationship::orphaned);
				comment_tokens.push_back(tok);
			}
		}
		index.finalize();

		REQUIRE(comment_tokens.size() == 3);
		// Verify comments are what we expect
		REQUIRE(comment_tokens[0]->to_string() == "/* a */");
		REQUIRE(comment_tokens[1]->to_string() == "/* b */");
		REQUIRE(comment_tokens[2]->to_string() == "/* c */");

		// Query between first and second comment (same line as the comments)
		auto line = comment_tokens[0].location().line();
		auto second_col = comment_tokens[1].location().column();
		tscc::lex::source_location query(source, line, second_col - 1, 0);

		auto before = index.find_before(query);
		REQUIRE(before.has_value());
		REQUIRE(before->token->to_string() == "/* a */");

		auto after = index.find_after(query);
		REQUIRE(after.has_value());
		REQUIRE(after->token->to_string() == "/* b */");
	}
}

TEST_CASE("Parse Errors", "[parser][errors]") {
	SECTION("Expected token error contains correct information") {
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::source_location loc(source, 10, 5, 100);

		tscc::parse::expected_token err(loc, "identifier", "number");

		REQUIRE(err.code() == tscc::error_code::ts1005);
		REQUIRE(err.location().line() == 10);
		REQUIRE(err.location().column() == 5);

		std::string msg = err.what();
		REQUIRE(msg.find("identifier") != std::string::npos);
		REQUIRE(msg.find("number") != std::string::npos);
	}

	// TODO: Once module_scope_state handles expression statements, add test
	// verifying that naked expressions like "42" parse successfully

	SECTION("TS1128: Unmatched closing brace at module scope") {
		std::stringstream input("}");
		auto source = std::make_shared<fake_source>("test.ts");
		tscc::lex::lexer lexer(input, source);
		tscc::parse::parser parser(lexer);

		try {
			parser.begin();
			FAIL("Expected declaration_or_statement_expected exception");
		} catch (const tscc::parse::declaration_or_statement_expected& e) {
			REQUIRE(e.code() == tscc::error_code::ts1128);
			REQUIRE(e.location().line() == 0);
			REQUIRE(e.location().column() == 0);
			REQUIRE(std::string(e.what()) ==
					"Declaration or statement expected.");
		}
	}
}