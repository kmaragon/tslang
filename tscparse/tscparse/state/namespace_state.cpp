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

#include "namespace_state.hpp"
#include <tsclex/tokens/close_brace_token.hpp>
#include "module_scope_visitor.hpp"
#include "namespace/namespace_header_state.hpp"
#include "state_result.hpp"

using namespace tscc;
using namespace tscc::parse::state;

namespace_state::namespace_state(lex::token keyword, bool ambient)
	: node_(std::make_unique<ast::namespace_node>(std::move(keyword))), ambient_(ambient) {}

namespace_state::namespace_state(lex::token declare_keyword, lex::token keyword)
	: node_(std::make_unique<ast::namespace_node>(std::move(keyword))), ambient_(true) {
	node_->declare_keyword_ = std::move(declare_keyword);
}

state_result namespace_state::process(parser& /*p*/, const lex::token& token) {
	if (!header_done_) {
		header_done_ = true;
		return state_result::push<namespace_header_state>(node_.get())
			.reprocess();
	}

	if (token.is<lex::tokens::close_brace_token>()) {
		node_->close_brace_ = token;
		return state_result::complete(std::move(node_));
	}

	return token.visit(module_scope_visitor{this, token.location(), token,
											ambient_, /*module_like=*/false});
}

accept_result namespace_state::accept_child(
	std::unique_ptr<ast::ast_node> child) {
	if (!child) {
		return accept_result::stay();
	}

	child = node_->adopt_child(std::move(child));
	node_->children_.emplace_back(std::move(child));
	return accept_result::stay();
}
