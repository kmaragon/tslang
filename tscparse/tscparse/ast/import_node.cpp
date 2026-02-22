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

#include "import_node.hpp"

using namespace tscc::parse::ast;

lexeme<std::string_view> import_specifier::name() const {
	if (name_.is<lex::tokens::default_token>())
		return {};
	return {&name_};
}

bool import_specifier::is_default() const noexcept {
	return name_.is<lex::tokens::default_token>();
}

const tscc::lex::token* import_specifier::type_keyword() const noexcept {
	return type_keyword_ ? &type_keyword_ : nullptr;
}

lexeme<std::string_view> import_specifier::alias() const {
	return alias_ ? lexeme<std::string_view>{&alias_}
				  : lexeme<std::string_view>{};
}

lexeme<std::string_view> import_attribute::key() const {
	return {&key_};
}

lexeme<std::string> import_attribute::value() const {
	return {&value_};
}

import_node::from_form& import_node::ensure_from() {
	if (std::holds_alternative<std::monostate>(form_))
		form_.emplace<from_form>();
	return std::get<from_form>(form_);
}

import_node::equals_form& import_node::ensure_equals() {
	if (std::holds_alternative<std::monostate>(form_))
		form_.emplace<equals_form>();
	return std::get<equals_form>(form_);
}

import_node::from_form::named_binding& import_node::ensure_named() {
	auto& f = ensure_from();
	if (std::holds_alternative<std::monostate>(f.secondary))
		f.secondary.emplace<from_form::named_binding>();
	return std::get<from_form::named_binding>(f.secondary);
}

import_node::equals_form::require_data& import_node::ensure_require() {
	auto& e = std::get<equals_form>(form_);
	if (std::holds_alternative<std::monostate>(e.rhs))
		e.rhs.emplace<equals_form::require_data>();
	return std::get<equals_form::require_data>(e.rhs);
}

import_node::attributes_data& import_node::ensure_attributes() {
	if (!attributes_)
		attributes_.emplace();
	return *attributes_;
}

import_node::import_node(lex::token import_keyword)
	: import_keyword_(std::move(import_keyword)) {}

const tscc::lex::token* import_node::import_keyword() const noexcept {
	return &import_keyword_;
}

const tscc::lex::token* import_node::type_keyword() const noexcept {
	return type_keyword_ ? &*type_keyword_ : nullptr;
}

lexeme<std::string_view> import_node::default_binding() const {
	auto* f = std::get_if<from_form>(&form_);
	if (f && f->default_binding)
		return {&*f->default_binding};
	return {};
}

lexeme<std::string_view> import_node::namespace_name() const {
	auto* f = std::get_if<from_form>(&form_);
	if (!f)
		return {};
	auto* ns = std::get_if<from_form::namespace_binding>(&f->secondary);
	if (ns)
		return {&ns->name};
	return {};
}

const std::vector<import_specifier>& import_node::named_specifiers()
	const noexcept {
	auto* f = std::get_if<from_form>(&form_);
	if (f) {
		auto* n = std::get_if<from_form::named_binding>(&f->secondary);
		if (n)
			return n->specifiers;
	}
	static const std::vector<import_specifier> empty;
	return empty;
}

lexeme<std::string> import_node::module_specifier() const {
	if (auto* s = std::get_if<side_effect_form>(&form_))
		return {&s->module_specifier};
	if (auto* f = std::get_if<from_form>(&form_))
		return {&f->module_specifier};
	return {};
}

const tscc::lex::token* import_node::attributes_keyword() const noexcept {
	return attributes_ ? &attributes_->keyword : nullptr;
}

const std::vector<import_attribute>& import_node::attributes() const noexcept {
	if (attributes_)
		return attributes_->entries;
	static const std::vector<import_attribute> empty;
	return empty;
}

lexeme<std::string_view> import_node::equals_name() const {
	auto* e = std::get_if<equals_form>(&form_);
	if (e)
		return {&e->name};
	return {};
}

lexeme<std::string> import_node::require_module_specifier() const {
	auto* e = std::get_if<equals_form>(&form_);
	if (!e)
		return {};
	auto* r = std::get_if<equals_form::require_data>(&e->rhs);
	if (r)
		return {&r->module_specifier};
	return {};
}

const qualified_name& import_node::entity_name() const noexcept {
	auto* e = std::get_if<equals_form>(&form_);
	if (e) {
		auto* q = std::get_if<qualified_name>(&e->rhs);
		if (q)
			return *q;
	}
	static const qualified_name empty;
	return empty;
}

const tscc::lex::source_location& import_node::location() const {
	return import_keyword_.location();
}

void import_node::visit_children(
	std::function<void(const ast_node*)> /*visitor*/) const {
	// import_node is a leaf - all data is in tokens, no child ast_nodes
}
