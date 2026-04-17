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

#include "declare_module_state.hpp"
#include <tsclex/tokens/close_brace_token.hpp>
#include "../../ast/exportable_node.hpp"
#include "../../error/export_assignment_conflicts.hpp"
#include "../module_scope_visitor.hpp"
#include "../state_result.hpp"
#include "declare_module_header_state.hpp"

using namespace tscc;
using namespace tscc::parse::state;

namespace {

const lex::token* named_export_keyword(const parse::ast::ast_node& node) {
	if (!parse::ast::ast_node::is_exportable(node.node_kind()))
		return nullptr;
	return static_cast<const parse::ast::exportable_node&>(node)
		.export_keyword();
}

}  // namespace

declare_module_state::declare_module_state(lex::token declare_keyword,
										   lex::token module_keyword)
	: node_(std::make_unique<ast::declare_module_node>(
		  std::move(declare_keyword), std::move(module_keyword))) {}

state_result declare_module_state::process(parser& /*p*/,
										   const lex::token& token) {
	if (!header_done_) {
		header_done_ = true;
		return state_result::push<declare_module_header_state>(node_.get())
			.reprocess();
	}

	if (token.is<lex::tokens::close_brace_token>()) {
		node_->close_brace_ = token;
		return state_result::complete(std::move(node_));
	}

	return token.visit(module_scope_visitor{this, token.location(), token,
										   /*ambient=*/true,
										   /*module_like=*/true});
}

accept_result declare_module_state::accept_child(
	std::unique_ptr<ast::ast_node> child) {
	if (!child) {
		return accept_result::stay();
	}

	if (child->node_kind() == parse::ast::ast_node::kind::export_assignment) {
		if (has_export_assignment_ || has_named_export_ || has_default_export_)
			throw export_assignment_conflicts(
				static_cast<const parse::ast::exportable_node&>(*child)
					.export_keyword()->location());
		has_export_assignment_ = true;
	} else if (auto* ek = named_export_keyword(*child)) {
		if (has_export_assignment_)
			throw export_assignment_conflicts(ek->location());
		has_named_export_ = true;
	}
	// TODO: check for export_default_node (TS2309, TS2528)

	node_->children_.emplace_back(node_->adopt_child(std::move(child)));
	return accept_result::stay();
}
