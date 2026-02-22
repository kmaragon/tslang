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

#include "import_state.hpp"
#include "import/initial_import_states.hpp"
#include "state_result.hpp"

using namespace tscc::parse::state;

import_state::import_state(lex::token import_keyword, bool equals_only)
	: node_(std::make_unique<ast::import_node>(std::move(import_keyword))),
	  builder_(node_.get()),
	  equals_only_(equals_only) {}

state_result import_state::process(parser& /*p*/, const lex::token& /*token*/) {
	return state_result::push<after_import_state>(&builder_, equals_only_)
		.reprocess();
}

accept_result import_state::accept_child(std::unique_ptr<ast::ast_node>) {
	return accept_result::complete(std::move(node_));
}
