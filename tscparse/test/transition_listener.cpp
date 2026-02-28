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

#include <cstdlib>
#include <memory>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <tscparse/parser.hpp>
#include "transition_logger.hpp"

namespace {

class transition_listener final : public Catch::EventListenerBase {
public:
	using Catch::EventListenerBase::EventListenerBase;

	void testRunStarting(Catch::TestRunInfo const&) override {
		if (const auto* path = std::getenv("TSCC_TRANSITION_LOG")) {
			logger_ = std::make_unique<tscc::parse::test::transition_logger>(path);
			tscc::parse::parser::set_observer(logger_.get());
		}
	}

	void testRunEnded(Catch::TestRunStats const&) override {
		if (logger_) {
			tscc::parse::parser::set_observer(nullptr);
			logger_.reset();
		}
	}

private:
	std::unique_ptr<tscc::parse::test::transition_logger> logger_;
};

}  // namespace

CATCH_REGISTER_LISTENER(transition_listener)
