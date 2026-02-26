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

#include <memory>
#include <tsclex/token.hpp>
#include "../../ast/type/type_parameter_node.hpp"
#include "../parser_state.hpp"

namespace tscc::parse::state {

/**
 * \brief Parses a single type parameter
 *
 * Handles: [in] [out] Identifier [extends Type] [= Type]
 * Enforces ordering constraints: `in` must precede `out`,
 * duplicates are rejected.
 */
class type_parameter_state : public parser_state {
public:
	type_parameter_state();

	state_result process(parser& p, const lex::token& token) override;

	accept_result accept_child(std::unique_ptr<ast::ast_node> child) override;

private:
	std::unique_ptr<ast::type_parameter_node> node_;
	lex::token in_token_;
	lex::token out_token_;
	bool has_name_ = false;
	bool constraint_pending_ = false;
	bool default_pending_ = false;
};

}  // namespace tscc::parse::state
