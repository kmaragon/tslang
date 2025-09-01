/**
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

#include "disjunction.hpp"
#include "alternative.hpp"

using namespace tsccore::regex;

disjunction::disjunction() : index_(0) {}

disjunction::disjunction(const disjunction& other) : disjunction() {
	if (other.index_ == 1) {
		new (reinterpret_cast<alternative*>(std::addressof(storage_)))
			alternative(*reinterpret_cast<const alternative*>(
				std::addressof(other.storage_)));
		index_ = 1;
	} else if (other.index_ == 2) {
		new (reinterpret_cast<std::vector<alternative>*>(
			std::addressof(storage_)))
			std::vector(*reinterpret_cast<const std::vector<alternative>*>(
				other.storage_));
		index_ = 2;
	}
}

disjunction::disjunction(disjunction&& other) noexcept : disjunction() {
	if (other.index_ == 1) {
		new (reinterpret_cast<alternative*>(std::addressof(storage_)))
			alternative(std::move(*reinterpret_cast<alternative*>(
				std::addressof(other.storage_))));
		index_ = 1;
	} else if (other.index_ == 2) {
		new (reinterpret_cast<std::vector<alternative>*>(
			std::addressof(storage_)))
			std::vector(std::move(
				*reinterpret_cast<std::vector<alternative>*>(other.storage_)));
		index_ = 2;
	}

	other.reset();
}

disjunction::~disjunction() {
	reset();
}

disjunction& disjunction::operator=(const disjunction& other) {
	if (&other == this)
		return *this;

	reset();
	if (other.index_ == 1) {
		new (reinterpret_cast<alternative*>(std::addressof(storage_)))
			alternative(*reinterpret_cast<const alternative*>(
				std::addressof(other.storage_)));
		index_ = 1;
	} else if (other.index_ == 2) {
		new (reinterpret_cast<std::vector<alternative>*>(
			std::addressof(storage_)))
			std::vector(*reinterpret_cast<const std::vector<alternative>*>(
				other.storage_));
		index_ = 2;
	}

	return *this;
}

disjunction& disjunction::operator=(disjunction&& other) noexcept {
	if (&other == this)
		return *this;

	reset();
	if (other.index_ == 1) {
		new (reinterpret_cast<alternative*>(std::addressof(storage_)))
			alternative(std::move(*reinterpret_cast<alternative*>(
				std::addressof(other.storage_))));
		index_ = 1;
	} else if (other.index_ == 2) {
		new (reinterpret_cast<std::vector<alternative>*>(
			std::addressof(storage_)))
			std::vector(std::move(
				*reinterpret_cast<std::vector<alternative>*>(other.storage_)));
		index_ = 2;
	}

	other.reset();
	return *this;
}

void disjunction::reset() noexcept {
	switch (index_) {
		case 0:
			return;
		case 1:
			reinterpret_cast<alternative*>(std::addressof(storage_))
				->~alternative();
			break;
		case 2:
			reinterpret_cast<std::vector<alternative>*>(
				std::addressof(storage_))
				->~vector();
			break;
	}

	index_ = 0;
}

std::span<const alternative> disjunction::get_alternatives() const {
	switch (index_) {
		case 0:
			return {};
		case 1:
			return {
				reinterpret_cast<const alternative*>(std::addressof(storage_)),
				1};
		case 2:
			return *reinterpret_cast<const std::vector<alternative>*>(
				std::addressof(storage_));
		default:
			return {};
	}
}

void disjunction::add_alternative(alternative&& alternative) {
	if (index_ == 0) {
		new (reinterpret_cast<class alternative*>(std::addressof(
			storage_))) class alternative(std::move(alternative));
		index_ = 1;
		return;
	}

	if (index_ == 1) {
		std::vector<class alternative> vect;
		vect.reserve(2);
		vect.emplace_back(std::move(
			*reinterpret_cast<class alternative*>(std::addressof(storage_))));
		vect.emplace_back(std::move(alternative));
		reinterpret_cast<class alternative*>(std::addressof(storage_))
			->~alternative();
		new (reinterpret_cast<std::vector<class alternative>*>(
			std::addressof(storage_))) std::vector(std::move(vect));
		index_ = 2;
		return;
	}

	if (index_ == 2) {
		reinterpret_cast<std::vector<class alternative>*>(
			std::addressof(storage_))
			->emplace_back(std::move(alternative));
	}
}

std::size_t disjunction::string_size() const noexcept {
	auto span = get_alternatives();
	std::size_t result = 0;
	for (auto& a : span)
		result += a.string_size() + 1;

	if (result == 0)
		return 0;
	return result - 1;
}

void disjunction::to_string(std::u32string& to) const {
	auto span = get_alternatives();
	for (size_t i = 0; i < span.size(); ++i) {
		if (i)
			to += U'|';
		span[i].to_string(to);
	}
}

bool disjunction::operator==(const disjunction& other) const noexcept {
	auto alternatives = get_alternatives();
	auto other_alternatives = other.get_alternatives();

	return std::equal(alternatives.begin(), alternatives.end(),
					  other_alternatives.begin(), other_alternatives.end());
}

bool disjunction::operator!=(const disjunction& other) const noexcept {
	return !(*this == other);
}
