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

#include "../specifier/specifier_list_state.hpp"
#include "import_node_builder.hpp"

namespace tscc::parse::state {

/**
 * \brief Parser sub-state for the contents of a named import list
 *
 * Thin subclass of specifier_list_state that delegates storage
 * to the import_node_builder. Completes with nullptr since no
 * separate AST node is produced.
 */
class named_import_state : public specifier_list_state {
public:
	explicit named_import_state(import_node_builder* builder);

	void store_specifier(lex::token name, lex::token type_keyword,
						 lex::token alias) override;

protected:
	state_result on_list_complete() override;

private:
	import_node_builder* builder_;
};

}  // namespace tscc::parse::state
