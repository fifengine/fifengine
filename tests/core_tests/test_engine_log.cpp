// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <filesystem>
#include <fstream>
#include <string>

#include "controller/engine.h"
#include "util/log/config.hpp"
#include "util/log/logger.h"
#include <catch2/catch_test_macros.hpp>              // NOLINT(build/include_alpha)
#include <catch2/matchers/catch_matchers_string.hpp> // NOLINT(build/include_alpha)

TEST_CASE("Engine logs requested, effective and detailed render backend info", "[engine][log]")
{
    auto& lm               = FIFE::LogManager::instance();
    bool const prevPrompt  = lm.isLogToPrompt();
    bool const prevFile    = lm.isLogToFile();
    auto const prevLevel   = lm.getLevelFilter();
    bool const prevVisible = lm.isVisible(LM_CONTROLLER);

    namespace fs       = std::filesystem;
    fs::path const dir = fs::temp_directory_path() / "fife-engine-log-test";
    fs::remove_all(dir);
    fs::create_directories(dir);
    fs::path const file = dir / "engine.log";

    FIFE::LogConfig cfg;
    cfg.console_enabled = false;
    cfg.file_enabled    = true;
    cfg.file_path       = file.string();
    lm.configure(cfg);
    lm.addVisibleModule(LM_CONTROLLER);
    lm.setLevelFilter(FIFE::LogManager::LEVEL_DEBUG);

    try {
        {
            FIFE::Engine engine;
            engine.getSettings().setRenderBackend("DUMMY_NULL");
            engine.getSettings().setScreenWidth(320);
            engine.getSettings().setScreenHeight(240);
            engine.getSettings().setFullScreen(false);
            engine.init();
            CHECK(engine.getRenderBackend()->getName() == "DUMMY_NULL");
            engine.destroy();
        }

        // Force spdlog to flush everything to disk.
        FIFE::Logger log(LM_CONTROLLER);
        lm.addVisibleModule(LM_CONTROLLER);
        FL_ERR(log, "flush-engine-log");

        std::ifstream in(file);
        REQUIRE(in.good());
        std::string const contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

        using Catch::Matchers::ContainsSubstring;
        using Catch::Matchers::Matches;

        CHECK_THAT(contents, Matches(R"([\s\S]*Requested render backend: DUMMY_NULL[\s\S]*)"));
        CHECK_THAT(contents, Matches(R"([\s\S]*Active render backend: DUMMY_NULL[\s\S]*)"));
        CHECK_THAT(contents, ContainsSubstring("Renderer: headless (no window)"));
        CHECK_THAT(contents, ContainsSubstring("Backend settings:"));
        // Displays are only enumerated in the windowed path; DUMMY_NULL skips that.
        CHECK_THAT(contents, ContainsSubstring("Available video drivers:"));
        CHECK_THAT(contents, ContainsSubstring("Effective video driver:"));
    } catch (...) {
        lm.setLevelFilter(prevLevel);
        lm.setLogToPrompt(prevPrompt);
        lm.setLogToFile(prevFile);
        if (!prevVisible) {
            lm.removeVisibleModule(LM_CONTROLLER);
        }
        std::error_code ec;
        fs::remove_all(dir, ec);
        throw;
    }

    lm.setLevelFilter(prevLevel);
    lm.setLogToPrompt(prevPrompt);
    lm.setLogToFile(prevFile);
    if (!prevVisible) {
        lm.removeVisibleModule(LM_CONTROLLER);
    }
    std::error_code ec;
    fs::remove_all(dir, ec);
}
