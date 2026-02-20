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

#include "import_node_builder.hpp"
#include "../token_helpers.hpp"

using namespace tscc::parse::state;

import_node_builder::import_node_builder(ast::import_node* node)
	: node_(node) {}

void import_node_builder::set_type_keyword(lex::token tok) {
	node_->type_keyword_ = std::move(tok);
}

void import_node_builder::set_default_binding(lex::token tok) {
	detail::normalize_identifier(tok);
	node_->ensure_from().default_binding = std::move(tok);
}

void import_node_builder::set_namespace_name(lex::token tok) {
	detail::normalize_identifier(tok);
	auto& f = node_->ensure_from();
	f.secondary = ast::import_node::from_form::namespace_binding{
		std::move(tok)};
}

void import_node_builder::init_named_imports() {
	node_->ensure_named();
}

void import_node_builder::add_named_specifier(ast::import_specifier spec) {
	detail::normalize_identifier(spec.name);
	if (spec.alias) detail::normalize_identifier(*spec.alias);
	node_->ensure_named().specifiers.push_back(std::move(spec));
}

void import_node_builder::set_module_specifier(lex::token tok) {
	if (auto* f =
			std::get_if<ast::import_node::from_form>(&node_->form_)) {
		f->module_specifier = std::move(tok);
	} else {
		node_->form_.emplace<ast::import_node::side_effect_form>(
			ast::import_node::side_effect_form{std::move(tok)});
	}
}

void import_node_builder::set_attributes_keyword(lex::token tok) {
	node_->ensure_attributes().keyword = std::move(tok);
}

void import_node_builder::add_attribute(ast::import_attribute attr) {
	detail::normalize_identifier(attr.key);
	node_->ensure_attributes().entries.push_back(std::move(attr));
}

void import_node_builder::set_equals_name(lex::token tok) {
	detail::normalize_identifier(tok);
	node_->ensure_equals().name = std::move(tok);
}

void import_node_builder::init_require() {
	node_->ensure_require();
}

void import_node_builder::set_require_module_specifier(lex::token tok) {
	std::get<ast::import_node::equals_form::require_data>(
		std::get<ast::import_node::equals_form>(node_->form_).rhs)
		.module_specifier = std::move(tok);
}

void import_node_builder::add_entity_identifier(lex::token tok) {
	detail::normalize_identifier(tok);
	node_->ensure_entity().identifiers.push_back(std::move(tok));
}
