/*
 * TSCC - a Typescript Compiler
 * Copyright (c) 2025. Keef Aragon
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

#include "alternative.hpp"

using namespace tsccore::regex;

// term class implementation
term::term(assertion assertion) : value_(assertion) {}

term::term(atom atom, std::optional<quantifier> quantifier)
	: value_(std::make_pair(atom, quantifier)) {}

bool term::is_assertion() const {
	return std::holds_alternative<assertion>(value_);
}

const assertion& term::get_assertion() const {
	return std::get<assertion>(value_);
}

const atom& term::get_atom() const {
	return std::get<std::pair<atom, std::optional<quantifier>>>(value_).first;
}

std::size_t term::string_size() const noexcept {
	return is_assertion() ? get_assertion().string_size() : get_atom().string_size() + (get_quantifier() ? get_quantifier()->string_size() : 0);
}

void term::to_string(std::u32string& to) const {
	if (is_assertion()) {
		get_assertion().to_string(to);
	} else {
		get_atom().to_string(to);
		if (get_quantifier()) {
			get_quantifier()->to_string(to);
		}
	}
}


bool term::operator==(const term& other) const noexcept {
	return value_ == other.value_;
}

bool term::operator!=(const term& other) const noexcept {
	return !(*this == other);
}

const std::optional<quantifier>& term::get_quantifier() const {
	return std::get<std::pair<atom, std::optional<quantifier>>>(value_).second;
}

// alternative class implementation
alternative::alternative(std::vector<term> terms) : terms_(std::move(terms)) {}

const std::vector<term>& alternative::get_terms() const {
	return terms_;
}

void alternative::add_term(term term) {
	terms_.push_back(std::move(term));
}

std::size_t alternative::string_size() const noexcept {
	std::size_t result = 0;
	for (auto& t : terms_) {
		result += t.string_size();
	}

	return result;
}

void alternative::to_string(std::u32string& to) const {
	for (auto& t : terms_) {
		t.to_string(to);
	}
}

bool alternative::operator==(const alternative& other) const noexcept {
	return terms_ == other.terms_;
}

bool alternative::operator!=(const alternative& other) const noexcept {
	return !(*this == other);
}
