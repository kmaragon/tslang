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
#include "lexeme.hpp"
#include "qualified_name.hpp"

namespace tscc::parse::state {
class import_node_builder;
}

namespace tscc::parse::ast {

/**
 * \brief A single specifier within a named import list
 *
 * Represents one element inside braces: [type] name [as alias]
 *
 * The name field holds an identifier_token (contextual keywords are
 * normalized), a constant_value_token (string literal), or a
 * default_token. The alias, when present, is always an identifier_token.
 */
class import_specifier {
	friend class state::import_node_builder;

	lex::token name_;
	lex::token type_keyword_;
	lex::token alias_;

public:
	/// Specifier name as a lexeme
	///
	/// Returns an invalid lexeme when the name is `default` —
	/// use is_default() for that case.
	lexeme<std::string_view> name() const;

	/// Whether this specifier uses the `default` keyword as its name
	bool is_default() const noexcept;

	/// Type-only modifier keyword, if present
	const lex::token* type_keyword() const noexcept;

	/// Alias binding after `as`, if present
	lexeme<std::string_view> alias() const;
};

/**
 * \brief A single key: value pair in an import attributes clause
 *
 * Represents one entry such as: type: "json"
 * The key is normalized to identifier_token when it was a contextual keyword.
 */
class import_attribute {
	friend class state::import_node_builder;

	lex::token key_;
	lex::token value_;

public:
	/// Attribute key (always normalized identifier)
	lexeme<std::string_view> key() const;

	/// Attribute value (always string literal)
	lexeme<std::string> value() const;
};

/**
 * \brief AST node for all forms of TypeScript import declarations
 *
 * Built incrementally by import_node_builder (a friend class in the
 * state namespace). Treat as const once parsing is complete.
 *
 * Stores only semantically meaningful tokens — syntactic punctuation
 * (commas, braces, parens, semicolons, etc.) is discarded during building.
 * Contextual keywords in identifier positions are normalized to
 * identifier_token.
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
	friend class state::import_node_builder;

public:
	/**
	 * \brief Construct an import node from its leading keyword
	 */
	explicit import_node(lex::token import_keyword);

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
	lexeme<std::string_view> default_binding() const;

	/**
	 * \brief Get the namespace binding name, if present
	 */
	lexeme<std::string_view> namespace_name() const;

	/**
	 * \brief Get all specifiers in the named import list
	 */
	const std::vector<import_specifier>& named_specifiers() const noexcept;

	/**
	 * \brief Get the module specifier string literal, if present
	 *
	 * The lexeme value is the module path as a UTF-8 std::string.
	 */
	lexeme<std::string> module_specifier() const;

	/**
	 * \brief Get the attributes keyword (assert/with), if present
	 */
	const lex::token* attributes_keyword() const noexcept;

	/**
	 * \brief Get all attributes in the attributes clause
	 */
	const std::vector<import_attribute>& attributes() const noexcept;

	/**
	 * \brief Get the binding name of an import-equals, if present
	 */
	lexeme<std::string_view> equals_name() const;

	/**
	 * \brief Get the module specifier inside require(), if present
	 *
	 * The lexeme value is the module path as a UTF-8 std::string.
	 */
	lexeme<std::string> require_module_specifier() const;

	/**
	 * \brief Get the qualified entity name of an import-equals, if present
	 */
	const qualified_name& entity_name() const noexcept;

	const lex::source_location& location() const override;
	void visit_children(
		std::function<void(const ast_node*)> visitor) const override;

private:
	struct side_effect_form {
		lex::token module_specifier;
	};

	struct from_form {
		std::optional<lex::token> default_binding;

		struct namespace_binding {
			lex::token name;
		};

		struct named_binding {
			std::vector<import_specifier> specifiers;
		};

		std::variant<std::monostate, namespace_binding, named_binding>
			secondary;

		lex::token module_specifier;
	};

	struct equals_form {
		lex::token name;

		struct require_data {
			lex::token module_specifier;
		};

		std::variant<std::monostate, require_data, qualified_name> rhs;
	};

	struct attributes_data {
		lex::token keyword;
		std::vector<import_attribute> entries;
	};

	from_form& ensure_from();
	equals_form& ensure_equals();
	from_form::named_binding& ensure_named();
	equals_form::require_data& ensure_require();
	attributes_data& ensure_attributes();

	lex::token import_keyword_;
	std::optional<lex::token> type_keyword_;
	std::variant<std::monostate, side_effect_form, from_form, equals_form>
		form_;
	std::optional<attributes_data> attributes_;
};

}  // namespace tscc::parse::ast
