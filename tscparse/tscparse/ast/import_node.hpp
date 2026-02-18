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
#include <tsclex/token.hpp>
#include <variant>
#include <vector>
#include "ast_node.hpp"

namespace tscc::parse::ast {

/**
 * \brief A single specifier within a named import list
 *
 * Represents one element inside braces: [type] name [as alias]
 *
 * The name field can hold an identifier_token, string_token, or
 * default_token depending on the source syntax.
 */
struct import_specifier {
	std::optional<lex::token> type_keyword;
	lex::token name;
	std::optional<lex::token> as_keyword;
	std::optional<lex::token> alias;
};

/**
 * \brief A single key: value pair in an import attributes clause
 *
 * Represents one entry such as: type: "json"
 */
struct import_attribute {
	lex::token key;
	lex::token colon;
	lex::token value;
};

/**
 * \brief AST node for all forms of TypeScript import declarations
 *
 * Built incrementally by parser states via setters. Treat as const
 * once parsing is complete (see AST Node Mutation Convention in CLAUDE.md).
 *
 * Covers every import variation:
 *
 * Side-effect:
 *   import "module";
 *
 * From-style (with optional type modifier):
 *   import [type] defaultExport from "module";
 *   import [type] * as ns from "module";
 *   import [type] { a, b as c, type d } from "module";
 *   import [type] defaultExport, { a } from "module";
 *   import [type] defaultExport, * as ns from "module";
 *
 * Import attributes (on side-effect and from-style):
 *   import data from "data.json" with { type: "json" };
 *   import data from "data.json" assert { type: "json" };
 *
 * Import equals (CommonJS require):
 *   import [type] name = require("module");
 *
 * Import equals (namespace alias):
 *   import [type] name = Qualified.Name;
 */
class import_node : public ast_node {
public:
	/**
	 * \brief Construct an import node from its leading keyword
	 */
	explicit import_node(lex::token import_keyword);

	/**
	 * \brief Set the type-only modifier keyword
	 */
	void set_type_keyword(lex::token tok);

	/**
	 * \brief Set the default binding identifier
	 */
	void set_default_binding(lex::token tok);

	/**
	 * \brief Set the comma separating default binding from secondary bindings
	 */
	void set_binding_comma(lex::token tok);

	/**
	 * \brief Set the namespace binding tokens: * as name
	 */
	void set_namespace_binding(lex::token asterisk,
							   lex::token as_keyword,
							   lex::token name);

	/**
	 * \brief Set the opening brace of a named import list
	 */
	void set_named_open_brace(lex::token tok);

	/**
	 * \brief Add a specifier to the named import list
	 */
	void add_named_specifier(import_specifier specifier);

	/**
	 * \brief Add a comma separator within the named import list
	 */
	void add_named_comma(lex::token tok);

	/**
	 * \brief Set the closing brace of a named import list
	 */
	void set_named_close_brace(lex::token tok);

	/**
	 * \brief Set the from keyword token
	 */
	void set_from_keyword(lex::token tok);

	/**
	 * \brief Set the module specifier string literal
	 *
	 * Used for both from-style imports and side-effect imports.
	 */
	void set_module_specifier(lex::token tok);

	/**
	 * \brief Set the import attributes keyword (assert or with)
	 */
	void set_attributes_keyword(lex::token tok);

	/**
	 * \brief Set the opening brace of the attributes clause
	 */
	void set_attributes_open_brace(lex::token tok);

	/**
	 * \brief Add an attribute to the attributes clause
	 */
	void add_attribute(import_attribute attr);

	/**
	 * \brief Add a comma separator within the attributes clause
	 */
	void add_attribute_comma(lex::token tok);

	/**
	 * \brief Set the closing brace of the attributes clause
	 */
	void set_attributes_close_brace(lex::token tok);

	/**
	 * \brief Set the binding name in an import-equals declaration
	 */
	void set_equals_name(lex::token tok);

	/**
	 * \brief Set the equals token in an import-equals declaration
	 */
	void set_equals(lex::token tok);

	/**
	 * \brief Set the require keyword in import-equals-require
	 */
	void set_require_keyword(lex::token tok);

	/**
	 * \brief Set the opening paren of the require call
	 */
	void set_require_open_paren(lex::token tok);

	/**
	 * \brief Set the module specifier inside the require call
	 */
	void set_require_module_specifier(lex::token tok);

	/**
	 * \brief Set the closing paren of the require call
	 */
	void set_require_close_paren(lex::token tok);

	/**
	 * \brief Add an identifier segment to an import-equals entity name
	 */
	void add_entity_identifier(lex::token tok);

	/**
	 * \brief Add a dot separator in an import-equals entity name
	 */
	void add_entity_dot(lex::token tok);

	/**
	 * \brief Set the trailing semicolon
	 */
	void set_semicolon(lex::token tok);

	/**
	 * \brief Get the import keyword token
	 */
	const lex::token* import_keyword() const noexcept;

	/**
	 * \brief Get the type-only modifier keyword, if present
	 */
	const lex::token* type_keyword() const noexcept;

	/**
	 * \brief Get the default binding identifier, if present
	 */
	const lex::token* default_binding() const noexcept;

	/**
	 * \brief Get the comma after default binding, if present
	 */
	const lex::token* binding_comma() const noexcept;

	/**
	 * \brief Get the asterisk token of a namespace binding, if present
	 */
	const lex::token* namespace_asterisk() const noexcept;

	/**
	 * \brief Get the as keyword of a namespace binding, if present
	 */
	const lex::token* namespace_as_keyword() const noexcept;

	/**
	 * \brief Get the name of a namespace binding, if present
	 */
	const lex::token* namespace_name() const noexcept;

	/**
	 * \brief Get the opening brace of named imports, if present
	 */
	const lex::token* named_open_brace() const noexcept;

	/**
	 * \brief Get all specifiers in the named import list
	 */
	const std::vector<import_specifier>& named_specifiers() const noexcept;

	/**
	 * \brief Get all comma separators in the named import list
	 */
	const std::vector<lex::token>& named_commas() const noexcept;

	/**
	 * \brief Get the closing brace of named imports, if present
	 */
	const lex::token* named_close_brace() const noexcept;

	/**
	 * \brief Get the from keyword, if present
	 */
	const lex::token* from_keyword() const noexcept;

	/**
	 * \brief Get the module specifier string literal, if present
	 */
	const lex::token* module_specifier() const noexcept;

	/**
	 * \brief Get the attributes keyword (assert/with), if present
	 */
	const lex::token* attributes_keyword() const noexcept;

	/**
	 * \brief Get the opening brace of the attributes clause, if present
	 */
	const lex::token* attributes_open_brace() const noexcept;

	/**
	 * \brief Get all attributes in the attributes clause
	 */
	const std::vector<import_attribute>& attributes() const noexcept;

	/**
	 * \brief Get all comma separators in the attributes clause
	 */
	const std::vector<lex::token>& attribute_commas() const noexcept;

	/**
	 * \brief Get the closing brace of the attributes clause, if present
	 */
	const lex::token* attributes_close_brace() const noexcept;

	/**
	 * \brief Get the binding name of an import-equals, if present
	 */
	const lex::token* equals_name() const noexcept;

	/**
	 * \brief Get the equals token of an import-equals, if present
	 */
	const lex::token* equals() const noexcept;

	/**
	 * \brief Get the require keyword, if present
	 */
	const lex::token* require_keyword() const noexcept;

	/**
	 * \brief Get the opening paren of the require call, if present
	 */
	const lex::token* require_open_paren() const noexcept;

	/**
	 * \brief Get the module specifier inside require(), if present
	 */
	const lex::token* require_module_specifier() const noexcept;

	/**
	 * \brief Get the closing paren of the require call, if present
	 */
	const lex::token* require_close_paren() const noexcept;

	/**
	 * \brief Get identifier segments of an import-equals entity name
	 */
	const std::vector<lex::token>& entity_identifiers() const noexcept;

	/**
	 * \brief Get dot separators of an import-equals entity name
	 */
	const std::vector<lex::token>& entity_dots() const noexcept;

	/**
	 * \brief Get the trailing semicolon, if present
	 */
	const lex::token* semicolon() const noexcept;

	const lex::source_location& location() const override;
	void visit_children(
		std::function<void(const ast_node*)> visitor) const override;

private:
	struct side_effect_form {
		lex::token module_specifier;
	};

	struct from_form {
		std::optional<lex::token> default_binding;
		std::optional<lex::token> binding_comma;

		struct namespace_binding {
			lex::token asterisk;
			lex::token as_keyword;
			lex::token name;
		};
		struct named_binding {
			lex::token open_brace;
			std::vector<import_specifier> specifiers;
			std::vector<lex::token> commas;
			lex::token close_brace;
		};
		std::variant<std::monostate, namespace_binding, named_binding>
			secondary;

		lex::token from_keyword;
		lex::token module_specifier;
	};

	struct equals_form {
		lex::token name;
		lex::token equals;

		struct require_data {
			lex::token keyword;
			lex::token open_paren;
			lex::token module_specifier;
			lex::token close_paren;
		};
		struct entity_data {
			std::vector<lex::token> identifiers;
			std::vector<lex::token> dots;
		};
		std::variant<std::monostate, require_data, entity_data> rhs;
	};

	struct attributes_data {
		lex::token keyword;
		lex::token open_brace;
		std::vector<import_attribute> entries;
		std::vector<lex::token> commas;
		lex::token close_brace;
	};

	from_form& ensure_from();
	equals_form& ensure_equals();
	from_form::named_binding& ensure_named();
	equals_form::require_data& ensure_require();
	equals_form::entity_data& ensure_entity();
	attributes_data& ensure_attributes();

	lex::token import_keyword_;
	std::optional<lex::token> type_keyword_;
	std::variant<std::monostate, side_effect_form, from_form, equals_form>
		form_;
	std::optional<attributes_data> attributes_;
	std::optional<lex::token> semicolon_;
};

}  // namespace tscc::parse::ast