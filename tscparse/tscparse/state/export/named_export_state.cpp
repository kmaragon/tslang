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

#include "named_export_state.hpp"
#include <tsclex/tokens/from_token.hpp>
#include <tsclex/tokens/newline_token.hpp>
#include <tsclex/tokens/semicolon_token.hpp>
#include "../specifier/specifier_list_state.hpp"
#include "../state_result.hpp"
#include "named_export_chain_states.hpp"

using namespace tscc::parse::state;

namespace tscc::parse::state {
namespace {

/**
 * \brief Specifier list sub-state that writes directly to a named_export_node
 */
class export_specifier_list_state : public specifier_list_state {
public:
	explicit export_specifier_list_state(named_export_state* parent)
		: parent_(parent) {}

	void store_specifier(lex::token name, lex::token type_keyword,
						 lex::token alias) override {
		parent_->add_specifier(std::move(name), std::move(type_keyword),
							   std::move(alias));
	}

protected:
	state_result on_list_complete() override {
		return state_result::complete(nullptr);
	}

private:
	named_export_state* parent_;
};

}  // namespace
}  // namespace tscc::parse::state

named_export_state::named_export_state()
	: node_(std::make_unique<ast::named_export_node>()) {}

void named_export_state::add_specifier(lex::token name, lex::token type_keyword,
									   lex::token alias) {
	auto& s = node_->specifiers_.emplace_back();
	s.name_ = std::move(name);
	s.type_keyword_ = std::move(type_keyword);
	s.alias_ = std::move(alias);
}

void named_export_state::set_module_specifier(lex::token tok) {
	node_->module_specifier_ = std::move(tok);
}

void named_export_state::set_semicolon(lex::token tok) {
	node_->semicolon_ = std::move(tok);
}

state_result named_export_state::process(parser& /*p*/,
										 const lex::token& token) {
	if (token.is<lex::tokens::newline_token>()) return state_result::stay();

	if (!list_done_) {
		return state_result::push<export_specifier_list_state>(this)
			.reprocess();
	}

	if (token.is<lex::tokens::from_token>()) {
		return state_result::push<named_export_module_spec_state>(this);
	}
	if (token.is<lex::tokens::semicolon_token>()) {
		set_semicolon(token);
		return state_result::complete(std::move(node_));
	}
	return state_result::complete(std::move(node_)).reprocess();
}

accept_result named_export_state::accept_child(
	std::unique_ptr<ast::ast_node> /*child*/) {
	if (!list_done_) {
		list_done_ = true;
		return accept_result::stay();
	}
	return accept_result::complete(std::move(node_));
}

std::optional<state_result> named_export_state::on_eof() {
	if (list_done_) return state_result::complete(std::move(node_));
	return std::nullopt;
}
