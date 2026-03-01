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

#include <optional>
#include <tsclex/source_location.hpp>
#include "parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Base class for parser states whose constructs can span multiple lines
 *
 * Intercepts newline tokens and tracks their location. When the subclass
 * gives up on a non-continuation token (complete + reprocess), the stored
 * newline location is attached to the result so the parser can restore
 * the synthetic newline for the parent state's ASI handling.
 *
 * Subclasses override process_content() instead of process().
 */
class multiline_state : public parser_state {
public:
	state_result process(parser& p, const lex::token& token) final override;

protected:
	virtual state_result process_content(parser& p,
										 const lex::token& token) = 0;

private:
	std::optional<lex::source_location> newline_loc_;
};

}  // namespace tscc::parse::state
