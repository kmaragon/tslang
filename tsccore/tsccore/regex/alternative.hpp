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

#pragma once

#include <variant>
#include <vector>
#include <optional>
#include "assertion.hpp"
#include "atom.hpp"
#include "quantifier.hpp"

namespace tsccore::regex {

class term {
public:
	term(assertion assertion);
	term(atom atom, std::optional<quantifier> quantifier = std::nullopt);
	
	bool is_assertion() const;
	const assertion& get_assertion() const;
	const atom& get_atom() const;
	const std::optional<quantifier>& get_quantifier() const;

private:
	std::variant<assertion, std::pair<atom, std::optional<quantifier>>> value_;
};

class alternative {
public:
	alternative() = default;
	explicit alternative(std::vector<term> terms);
	
	const std::vector<term>& get_terms() const;
	void add_term(term term);

private:
	std::vector<term> terms_;
};
}  // namespace tsccore::regex