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

#include "multiline_state.hpp"
#include <tsclex/tokens/newline_token.hpp>
#include "state_result.hpp"

using namespace tscc::parse::state;

state_result multiline_state::process(parser& p, const lex::token& token) {
	if (token.is<lex::tokens::newline_token>()) {
		newline_loc_ = token.location();
		return state_result::stay();
	}

	auto result = process_content(p, token);

	if (result.is_complete() && result.should_reprocess() && newline_loc_) {
		result.set_newline_location(std::move(*newline_loc_));
	}

	newline_loc_.reset();
	return result;
}
