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

#include "jsdoc_token.hpp"
#include <sstream>
#include <unordered_set>

#include "tsccore/json.hpp"

using namespace tscc::lex::tokens;

namespace {

const std::unordered_set<std::u32string_view>& inline_tags() {
	static std::unordered_set<std::u32string_view> result{
		{U"link", U"tutorial"}, 2};

	return result;
}

const std::unordered_set<std::u32string_view>& block_tags() {
	static std::unordered_set<std::u32string_view> result{
		{U"abstract",	 U"access",
		 U"alias",		 U"async",
		 U"augments",	 U"author",
		 U"borrows",	 U"class",
		 U"classdesc",	 U"constant",
		 U"constructs",	 U"copyright",
		 U"default",	 U"deprecated",
		 U"description", U"enum",
		 U"event",		 U"example",
		 U"exports",	 U"external",
		 U"file",		 U"fires",
		 U"function",	 U"generator",
		 U"global",		 U"hideconstructor",
		 U"ignore",		 U"implements",
		 U"inheritdoc",	 U"inner",
		 U"instance",	 U"interface",
		 U"kind",		 U"lends",
		 U"license",	 U"listens",
		 U"member",		 U"memberof",
		 U"mixes",		 U"mixin",
		 U"module",		 U"name",
		 U"namespace",	 U"override",
		 U"package",	 U"param",
		 U"private",	 U"property",
		 U"protected",	 U"public",
		 U"readonly",	 U"requires",
		 U"returns",	 U"see",
		 U"since",		 U"static",
		 U"summary",	 U"this",
		 U"throws",		 U"todo",
		 U"tutorial",	 U"type",
		 U"typedef",	 U"variation",
		 U"version",	 U"yields"}};

	return result;
}

struct extract_jsdoc_part {
	template <typename T>
	const jsdoc_token::jsdoc_part* operator()(const T& etype) const {
		return static_cast<const jsdoc_token::jsdoc_part*>(&etype);
	}
};

struct write_jsdoc_part {
	std::ostream& os;

	template <typename T>
	std::ostream& operator()(const T& etype) const {
		return etype.write(os);
	}
};

}  // namespace

jsdoc_token::jsdoc_part_type jsdoc_token::jsdoc_string_part::type() const {
	return jsdoc_part_type::string_value;
}

bool jsdoc_token::jsdoc_string_part::equivalent(const jsdoc_part& other) const {
	if (other.type() != jsdoc_part_type::string_value)
		return false;

	return str_ == static_cast<const jsdoc_string_part&>(other).str_;
}

std::ostream& jsdoc_token::jsdoc_string_part::write(std::ostream& to) const {
	return to << to_json_string(str_, 0);
}

jsdoc_token::jsdoc_part_type jsdoc_token::jsdoc_tag_part::type() const {
	return jsdoc_part_type::tag;
}

bool jsdoc_token::jsdoc_tag_part::equivalent(const jsdoc_part& other) const {
	if (other.type() != jsdoc_part_type::tag)
		return false;

	return tag_ == static_cast<const jsdoc_tag_part&>(other).tag_;
}

std::ostream& jsdoc_token::jsdoc_tag_part::write(std::ostream& to) const {
	return to << "@" << tag_;
}

jsdoc_token::jsdoc_part_type jsdoc_token::jsdoc_type_part::type() const {
	return jsdoc_part_type::type_parameter;
}

bool jsdoc_token::jsdoc_type_part::equivalent(const jsdoc_part& other) const {
	if (other.type() != jsdoc_part_type::type_parameter)
		return false;

	return type_ == static_cast<const jsdoc_type_part&>(other).type_;
}

std::ostream& jsdoc_token::jsdoc_type_part::write(std::ostream& to) const {
	return to << "{" << to_json_string(type_, 0) << "}";
}

const jsdoc_token::jsdoc_part& jsdoc_token::jsdoc_line::operator[](
	std::size_t index) const {
	return *std::visit(extract_jsdoc_part{}, entries_[index]);
}

bool jsdoc_token::jsdoc_line::equivalent(const jsdoc_line& other) const {
	if (entries_.size() != other.entries_.size())
		return false;

	auto lit = entries_.begin();
	auto rit = other.entries_.begin();
	for (; lit != entries_.end(); ++lit, ++rit) {
		auto left = std::visit(extract_jsdoc_part{}, *lit);
		auto right = std::visit(extract_jsdoc_part{}, *rit);

		if (!left->equivalent(*right))
			return false;
	}

	return true;
}

std::ostream& jsdoc_token::jsdoc_line::write(std::ostream& to) const {
	auto b = entries_.begin();
	if (b == entries_.end()) {
		return to;
	}

	write_jsdoc_part writer{to};
	std::visit(writer, *b);

	++b;
	while (b != entries_.end()) {
		to << " ";
		std::visit(writer, *b);
		++b;
	}

	return to;
}

jsdoc_token::jsdoc_token(const std::span<std::u32string>& comment_lines) {
	lines_.reserve(comment_lines.size() * 2);
	for (const auto& comment_line : comment_lines) {
		jsdoc_line line;

		bool escape = false;
		std::u32string current_value;
		std::u32string_view current = comment_line;
		std::size_t at = 0;

		// trim any whitespace
		while (at < current.size() && std::iswspace(current[at]))
			++at;

		// see if we start with an asterisk
		while (at < current.size() && current[at] == '*')
			++at;

		current = current.substr(at);

		while (!current.empty()) {
			at = 0;

			// trim any whitespace
			while (at < current.size() && std::iswspace(current[at]))
				++at;

			current = current.substr(at);
			at = 0;

			current_value.reserve(current.size());
			while (at < current.size()) {
				if (escape) {
					escape = false;
					current_value.push_back(current[at]);
					continue;
				}

				if (current[at] == '\\')
					escape = true;

				if (current[at] == '@') {
					auto start_tag = at++;
					while (at < current.size() && std::isalpha(current[at]))
						++at;

					auto tag_name =
						current.substr(start_tag + 1, at - (start_tag + 1));
					auto tag_name_str = to_json_string(tag_name, 0);
					if (line.empty() && current_value.empty() &&
						block_tags().contains(tag_name)) {
						line.append(jsdoc_tag_part(std::move(tag_name_str)));
						break;
					}

					if (inline_tags().contains(tag_name)) {
						if (!current_value.empty()) {
							line.append(
								jsdoc_string_part(std::move(current_value)));
						}

						line.append(jsdoc_tag_part(std::move(tag_name_str)));
						break;
					}

					// just treat it like a normal string
					at = start_tag;
				}

				if (current[at] == '{') {
					auto start_type = at++;
					while (at < current.size() && current[at] != '}')
						++at;

					auto type_name =
						current.substr(start_type + 1, at - (start_type + 1));
					if (!type_name.empty()) {
						if (!current_value.empty()) {
							line.append(
								jsdoc_string_part(std::move(current_value)));
						}

						line.append(jsdoc_type_part{std::u32string{type_name}});
						++at;
						break;
					}
				}

				current_value.push_back(current[at++]);
			}

			current = current.substr(at);
		}

		if (!current_value.empty()) {
			line.append(jsdoc_string_part(std::move(current_value)));
		}

		lines_.emplace_back(std::move(line));
	}

	lines_.shrink_to_fit();
}

bool jsdoc_token::operator==(const jsdoc_token& other) const {
	if (lines_.size() != other.lines_.size())
		return false;

	auto lit = lines_.begin();
	auto rit = other.lines_.begin();
	for (; lit != lines_.end(); ++lit, ++rit) {
		if (!lit->equivalent(*rit))
			return false;
	}

	return true;
}

bool jsdoc_token::operator!=(const jsdoc_token& other) const {
	return !operator==(other);
}

std::string jsdoc_token::to_string() const {
	auto it = lines_.begin();
	if (it == lines_.end())
		return {};

	std::stringstream ss;
	ss << "/**";
	if (!it->empty()) {
		ss << " ";
	}
	it->write(ss);
	++it;

	for (; it != lines_.end(); ++it) {
		ss << "\n *";
		if (!it->empty()) {
			ss << " ";
		}
		it->write(ss);
	}

	ss << "/";

	return ss.str();
}