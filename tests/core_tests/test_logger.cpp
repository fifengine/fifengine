// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "util/log/logger.h"

// Standard C++ library includes
#include <string>

// Platform specific includes
#include "fife_unittest.h"

// FIFE includes
#include "modules.h"

using FIFE::LogConfig;
using FIFE::Logger;
using FIFE::LogManager;

TEST_CASE("LogManager singleton", "[core][logger]")
{
    auto& a = LogManager::instance();
    auto& b = LogManager::instance();
    CHECK(&a == &b);
}

TEST_CASE("LogManager module visibility", "[core][logger]")
{
    auto& lm = LogManager::instance();
    lm.clearVisibleModules();

    // initially nothing is visible
    CHECK(!lm.isVisible(LM_AUDIO));
    CHECK(!lm.isVisible(LM_VIEW));

    // adding a module makes it visible
    lm.addVisibleModule(LM_AUDIO);
    CHECK(lm.isVisible(LM_AUDIO));

    // parent module is not made visible by child
    CHECK(!lm.isVisible(LM_CORE));

    // child submodule (under GUI) — adding it also makes parent visible
    lm.addVisibleModule(LM_CONSOLE);
    CHECK(lm.isVisible(LM_CONSOLE));
    CHECK(lm.isVisible(LM_GUI));

    // removing a module
    lm.removeVisibleModule(LM_GUI);
    CHECK(!lm.isVisible(LM_GUI));
    CHECK(!lm.isVisible(LM_CONSOLE));

    lm.clearVisibleModules();
    CHECK(!lm.isVisible(LM_AUDIO));
    CHECK(!lm.isVisible(LM_VIEW));
}

TEST_CASE("LogManager level filter", "[core][logger]")
{
    auto& lm = LogManager::instance();
    lm.setLevelFilter(LogManager::LEVEL_WARN);
    CHECK(lm.getLevelFilter() == LogManager::LEVEL_WARN);

    lm.setLevelFilter(LogManager::LEVEL_DEBUG);
    CHECK(lm.getLevelFilter() == LogManager::LEVEL_DEBUG);
}

TEST_CASE("Logger construction and getModule", "[core][logger]")
{
    Logger log(LM_AUDIO);
    CHECK(log.getModule() == LM_AUDIO);

    Logger log2(LM_VIEWVIEW);
    CHECK(log2.getModule() == LM_VIEWVIEW);
}

TEST_CASE("Logger log does not crash", "[core][logger]")
{
    Logger log(LM_CONTROLLER);
    // basic log call — should not throw or crash
    log.log(LogManager::LEVEL_LOG, "test message");

    // log with formatting
    log.log(LogManager::LEVEL_DEBUG, "debug message");
    log.log(LogManager::LEVEL_WARN, "warning");
    log.log(LogManager::LEVEL_ERROR, "error");

    // FL_* macros compile and work
    FL_DBG(log, "dbg");
    FL_LOG(log, "log");
    FL_WARN(log, "warn");
    FL_ERR(log, "err");
}

TEST_CASE("LogManager getModuleName", "[core][logger]")
{
    auto& lm = LogManager::instance();

    CHECK(std::string(lm.getModuleName(LM_AUDIO)) == "Audio");
    CHECK(std::string(lm.getModuleName(LM_MODEL)) == "Model");
    CHECK(std::string(lm.getModuleName(LM_VIEWVIEW)) == "View::View");

    // out of range module
    CHECK(std::string(lm.getModuleName(static_cast<logmodule_t>(-1))) == "Unknown");
}

TEST_CASE("LogManager log output toggles", "[core][logger]")
{
    auto& lm = LogManager::instance();

    lm.setLogToPrompt(false);
    CHECK(!lm.isLogToPrompt());
    lm.setLogToPrompt(true);
    CHECK(lm.isLogToPrompt());

    lm.setLogToFile(false);
    CHECK(!lm.isLogToFile());
}

TEST_CASE("LogManager configure sets sinks correctly", "[core][logger]")
{
    auto& lm = LogManager::instance();

    // Configure with console-only
    LogConfig cfg;
    cfg.console_enabled = true;
    cfg.file_enabled    = false;
    lm.configure(cfg);
    CHECK(lm.isLogToPrompt());
    CHECK(!lm.isLogToFile());

    // Configure with file-only
    cfg.console_enabled = false;
    cfg.file_enabled    = true;
    lm.configure(cfg);
    CHECK(!lm.isLogToPrompt());
    CHECK(lm.isLogToFile());

    // Restore defaults
    cfg.console_enabled = true;
    cfg.file_enabled    = false;
    lm.configure(cfg);
    CHECK(lm.isLogToPrompt());
    CHECK(!lm.isLogToFile());
}
