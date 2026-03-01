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

#include "../parser_state.hpp"
#include "import_node_builder.hpp"

namespace tscc::parse::state {

/**
 * \brief Handles completion after a module specifier string
 *
 * Accepts semicolon, with/assert (pushing import_attributes_state),
 * or ASI (complete with reprocess). After attributes complete,
 * accepts only semicolon or ASI.
 */
class after_module_spec_state : public parser_state {
public:
	explicit after_module_spec_state(import_node_builder* builder);

	state_result process(parser& p, const lex::token& token) override;
	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;
	std::optional<state_result> on_eof() override;

private:
	import_node_builder* builder_;

	enum class mode { initial, awaiting_sub, post_sub };
	mode mode_ = mode::initial;
	bool saw_newline_ = false;

	class initial_visitor;
	class post_sub_visitor;
};

}  // namespace tscc::parse::state
