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

#include "../../ast/import_node.hpp"

namespace tscc::parse::state {

/**
 * \brief Incrementally builds an import_node during parsing
 *
 * Friend of import_node; provides the mutation API that parser states
 * use to populate the node.  Contextual keywords in identifier
 * positions are normalized to identifier_token automatically.
 * Purely syntactic tokens (commas, braces, parens, semicolons, etc.)
 * are not stored.
 */
class import_node_builder {
public:
	explicit import_node_builder(ast::import_node* node);

	void set_type_keyword(lex::token tok);
	void set_default_binding(lex::token tok);
	void set_namespace_name(lex::token tok);
	void init_named_imports();
	void add_named_specifier(lex::token name, lex::token type_keyword,
							 lex::token alias);
	void set_module_specifier(lex::token tok);
	void set_attributes_keyword(lex::token tok);
	void add_attribute(lex::token key, lex::token value);
	void set_equals_name(lex::token tok);
	void init_require();
	void set_require_module_specifier(lex::token tok);
	void add_entity_identifier(lex::token tok);

private:
	ast::import_node* node_;
};

}  // namespace tscc::parse::state
