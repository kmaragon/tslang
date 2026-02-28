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

#include "transition_logger.hpp"
#include <cxxabi.h>
#include <cstdlib>
#include <string>
#include <thread>
#include <typeinfo>
#include <tsclex/token.hpp>
#include <tscparse/ast/ast_node.hpp>
#include <tscparse/state/parser_state.hpp>

using namespace tscc::parse::test;

namespace {

// Extract the unqualified class name from a demangled type name.
// E.g. "tscc::parse::state::import_state" -> "import_state"
std::string short_name(const std::type_info& ti) {
	int status = 0;
	char* demangled = abi::__cxa_demangle(ti.name(), nullptr, nullptr, &status);
	std::string full = (status == 0 && demangled) ? demangled : ti.name();
	std::free(demangled);

	// Strip everything up to and including the last "::"
	if (auto pos = full.rfind("::"); pos != std::string::npos) {
		return full.substr(pos + 2);
	}
	return full;
}

}  // namespace

transition_logger::transition_logger(std::string_view output_path)
	: out_(std::string(output_path)) {}

transition_logger::~transition_logger() = default;

void transition_logger::on_push(const state::parser_state& source,
								const lex::token& token,
								const state::parser_state& target) {
	auto source_name = short_name(typeid(source));
	auto token_name = token.visit(
		[](const auto& tok) { return short_name(typeid(tok)); });
	auto target_name = short_name(typeid(target));

	out_ << std::this_thread::get_id() << '\t' << "push\t" << source_name
		 << '\t' << token_name << '\t' << target_name << '\n';
}

void transition_logger::on_complete(const state::parser_state& state,
									const lex::token* token,
									const ast::ast_node* node) {
	log_completion("complete", state, token, node);
}

void transition_logger::on_cascade(const state::parser_state& state,
									const lex::token* token,
									const ast::ast_node* node) {
	log_completion("cascade", state, token, node);
}

void transition_logger::log_completion(std::string_view kind,
										const state::parser_state& state,
										const lex::token* token,
										const ast::ast_node* node) {
	auto state_name = short_name(typeid(state));
	std::string token_name;
	if (token) {
		token_name = token->visit(
			[](const auto& tok) { return short_name(typeid(tok)); });
	} else {
		token_name = "<eof>";
	}
	auto node_name = node ? short_name(typeid(*node)) : "<null>";

	out_ << std::this_thread::get_id() << '\t' << kind << '\t' << state_name
		 << '\t' << token_name << '\t' << node_name << '\n';
}
