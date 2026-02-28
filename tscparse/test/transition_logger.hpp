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

#include <fstream>
#include <string_view>
#include <tscparse/parser_observer.hpp>

namespace tscc::parse::test {

/**
 * \brief Logs parser state transitions to a file
 *
 * Each on_push() call appends a tab-separated line:
 *   source_state\ttoken_type\ttarget_state
 *
 * Uses RTTI (typeid + abi::__cxa_demangle) to extract type names.
 * Deduplication is left to the post-processor.
 */
class transition_logger final : public parser_observer {
public:
	explicit transition_logger(std::string_view output_path);
	~transition_logger() override;

	void on_push(const state::parser_state& source,
				 const lex::token& token,
				 const state::parser_state& target) override;

	void on_complete(const state::parser_state& state,
					 const lex::token* token,
					 const ast::ast_node* node) override;

	void on_cascade(const state::parser_state& state,
					 const lex::token* token,
					 const ast::ast_node* node) override;

private:
	void log_completion(std::string_view kind,
						const state::parser_state& state,
						const lex::token* token,
						const ast::ast_node* node);

	std::ofstream out_;
};

}  // namespace tscc::parse::test
