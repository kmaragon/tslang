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

#include "type_parameter_state.hpp"
#include <tsclex/tokens/eq_token.hpp>
#include <tsclex/tokens/newline_token.hpp>
#include <tsclex/tokens/extends_token.hpp>
#include <tsclex/tokens/in_token.hpp>
#include <tsclex/tokens/out_token.hpp>
#include "../../error/expected_token.hpp"
#include "../state_result.hpp"
#include "../token_helpers.hpp"
#include "type_expression_state.hpp"

using namespace tscc;
using namespace tscc::parse::state;

type_parameter_state::type_parameter_state() = default;

state_result type_parameter_state::process(parser& /*p*/,
										   const lex::token& token) {
	if (token.is<lex::tokens::newline_token>()) return state_result::stay();

	if (!has_name_) {
		if (token.is<lex::tokens::in_token>()) {
			if (in_token_) {
				throw expected_token(token.location(), "identifier",
									 token->to_string());
			}
			if (out_token_) {
				throw expected_token(token.location(), "identifier",
									 token->to_string());
			}
			in_token_ = token;
			return state_result::stay();
		}
		if (token.is<lex::tokens::out_token>()) {
			if (out_token_) {
				throw expected_token(token.location(), "identifier",
									 token->to_string());
			}
			out_token_ = token;
			return state_result::stay();
		}

		if (!detail::can_be_identifier(token)) {
			throw expected_token(token.location(), "identifier",
								 token->to_string());
		}
		lex::token normalized = token;
		detail::normalize_identifier(normalized);
		node_ = std::make_unique<ast::type_parameter_node>(std::move(normalized));
		if (in_token_) {
			node_->in_keyword_ = std::move(in_token_);
		}
		if (out_token_) {
			node_->out_keyword_ = std::move(out_token_);
		}
		has_name_ = true;
		return state_result::stay();
	}

	if (token.is<lex::tokens::extends_token>() && !node_->constraint_) {
		node_->extends_ = token;
		constraint_pending_ = true;
		return state_result::push<type_expression_state>();
	}

	if (token.is<lex::tokens::eq_token>() && !node_->default_) {
		node_->equals_ = token;
		default_pending_ = true;
		return state_result::push<type_expression_state>();
	}

	return state_result::complete(std::move(node_)).reprocess();
}

accept_result type_parameter_state::accept_child(
	std::unique_ptr<ast::ast_node> child) {
	child = node_->adopt_child(std::move(child));
	auto type = std::unique_ptr<const ast::type_definition>(
		static_cast<const ast::type_definition*>(child.release()));

	if (constraint_pending_) {
		constraint_pending_ = false;
		node_->constraint_ = std::move(type);
		return accept_result::stay();
	}

	if (default_pending_) {
		default_pending_ = false;
		node_->default_ = std::move(type);
		return accept_result::stay();
	}

	throw std::logic_error("unexpected child in type_parameter_state");
}
