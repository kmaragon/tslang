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

#include "state_result.hpp"

namespace tscc::parse {

state_result state_result::stay() {
	state_result r;
	r.action_ = action::stay;
	return r;
}

state_result state_result::push(std::unique_ptr<parser_state> child) {
	state_result r;
	r.action_ = action::push;
	r.child_ = std::move(child);
	return r;
}

state_result state_result::complete(std::unique_ptr<ast::ast_node> node) {
	state_result r;
	r.action_ = action::complete;
	r.node_ = std::move(node);
	return r;
}

state_result state_result::reprocess() && {
	reprocess_ = true;
	return std::move(*this);
}

bool state_result::should_reprocess() const noexcept {
	return reprocess_;
}

bool state_result::is_stay() const noexcept {
	return action_ == action::stay;
}

bool state_result::is_push() const noexcept {
	return action_ == action::push;
}

bool state_result::is_complete() const noexcept {
	return action_ == action::complete;
}

std::unique_ptr<parser_state> state_result::take_child() && {
	return std::move(child_);
}

std::unique_ptr<ast::ast_node> state_result::take_node() && {
	return std::move(node_);
}

accept_result accept_result::stay() {
	accept_result r;
	r.action_ = action::stay;
	return r;
}

accept_result accept_result::complete(std::unique_ptr<ast::ast_node> node) {
	accept_result r;
	r.action_ = action::complete;
	r.node_ = std::move(node);
	return r;
}

bool accept_result::is_stay() const noexcept {
	return action_ == action::stay;
}

bool accept_result::is_complete() const noexcept {
	return action_ == action::complete;
}

std::unique_ptr<ast::ast_node> accept_result::take_node() && {
	return std::move(node_);
}

}  // namespace tscc::parse
