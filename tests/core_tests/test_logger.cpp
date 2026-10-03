// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "util/log/logger.h"

// Standard C++ library includes
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

// Platform specific includes
#include <catch2/catch_test_macros.hpp>

// FIFE includes
#include "modules.h"

using FIFE::LogConfig;
using FIFE::Logger;
using FIFE::LogManager;

TEST_CASE("LogManager singleton", "[core][logger]")
{
    auto const & a = LogManager::instance();
    auto const & b = LogManager::instance();
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

TEST_CASE("FIFE levels map to the matching spdlog levels", "[core][logger]")
{
#ifdef LOG_ENABLED
    // FIFE's LogLevel and spdlog's level_enum do not share a layout, so the
    // mapping is explicit. These assertions fail if the mapping is ever
    // replaced by a static_cast: spdlog has an extra `trace` at index 0,
    // which shifts every level one slot too low.
    static_assert(FIFE::toSpdlogLevel(LogManager::LEVEL_DEBUG) == spdlog::level::debug);
    static_assert(FIFE::toSpdlogLevel(LogManager::LEVEL_LOG) == spdlog::level::info);
    static_assert(FIFE::toSpdlogLevel(LogManager::LEVEL_WARN) == spdlog::level::warn);
    static_assert(FIFE::toSpdlogLevel(LogManager::LEVEL_ERROR) == spdlog::level::err);
    static_assert(FIFE::toSpdlogLevel(LogManager::LEVEL_PANIC) == spdlog::level::critical);

    CHECK(FIFE::toSpdlogLevel(LogManager::LEVEL_DEBUG) == spdlog::level::debug);
    CHECK(FIFE::toSpdlogLevel(LogManager::LEVEL_LOG) == spdlog::level::info);
    CHECK(FIFE::toSpdlogLevel(LogManager::LEVEL_WARN) == spdlog::level::warn);
    CHECK(FIFE::toSpdlogLevel(LogManager::LEVEL_ERROR) == spdlog::level::err);
    CHECK(FIFE::toSpdlogLevel(LogManager::LEVEL_PANIC) == spdlog::level::critical);

    // FIFE levels must never be spelled the same way as the spdlog value at
    // the same ordinal, which is what a static_cast would produce.
    CHECK(static_cast<int>(LogManager::LEVEL_WARN) != static_cast<int>(spdlog::level::warn));
    CHECK(static_cast<int>(LogManager::LEVEL_ERROR) != static_cast<int>(spdlog::level::err));

    // An unrecognised level degrades to the lowest severity rather than being
    // promoted to something the caller did not ask for.
    CHECK(FIFE::toSpdlogLevel(static_cast<LogManager::LogLevel>(200)) == spdlog::level::trace);
#endif
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

    // Silence console output for this test — we only care that it doesn't crash
    auto& lm        = LogManager::instance();
    bool const prev = lm.isLogToPrompt();
    lm.setLogToPrompt(false);

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

    lm.setLogToPrompt(prev);
}

TEST_CASE("Level filter and severity labels reach the sink", "[core][logger]")
{
#ifdef LOG_ENABLED
    namespace fs = std::filesystem;

    auto& lm = LogManager::instance();

    std::error_code ec;
    fs::path const log_dir = fs::temp_directory_path(ec) / "fife_logger_test";
    fs::remove_all(log_dir, ec);
    fs::create_directories(log_dir, ec);
    fs::path const log_file = log_dir / "level_filter.log";

    // Remember the shared singleton's state so we can hand it back intact.
    bool const prev_prompt = lm.isLogToPrompt();
    bool const prev_file   = lm.isLogToFile();
    auto const prev_level  = lm.getLevelFilter();

    LogConfig cfg;
    cfg.console_enabled = false;
    cfg.file_enabled    = true;
    cfg.file_path       = log_file.string();
    lm.configure(cfg);

    Logger log(LM_AUDIO);
    lm.addVisibleModule(LM_AUDIO);

    // A WARN threshold must drop debug and log messages...
    lm.setLevelFilter(LogManager::LEVEL_WARN);
    FL_DBG(log, "probe_debug");
    FL_LOG(log, "probe_log");
    FL_WARN(log, "probe_warning");

    // ...and lowering it again must let them through.
    lm.setLevelFilter(LogManager::LEVEL_DEBUG);
    FL_LOG(log, "probe_info");
    // Ends on an error so spdlog's flush_on(warn) pushes everything to disk.
    FL_ERR(log, "probe_error");

    // Release the file sink, which also closes and flushes it.
    cfg.console_enabled = prev_prompt;
    cfg.file_enabled    = false;
    lm.configure(cfg);

    std::ifstream in(log_file);
    std::string const contents{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};

    CHECK(contents.find("probe_warning") != std::string::npos);
    CHECK(contents.find("probe_info") != std::string::npos);
    CHECK(contents.find("probe_error") != std::string::npos);
    CHECK(contents.find("probe_debug") == std::string::npos);
    CHECK(contents.find("probe_log") == std::string::npos);

    // The %l placeholder must report the severity of the message on its own
    // line. Asserting per line rather than over the whole file matters: with
    // the old static_cast, FIFE LEVEL_WARN was labelled "info" and LEVEL_ERROR
    // was labelled "warning", so a whole-file search still finds a "warning"
    // and an "info" — just on the wrong lines.
    auto line_with = [&contents](std::string const & needle) {
        std::size_t const pos = contents.find(needle);
        if (pos == std::string::npos) {
            return std::string{};
        }
        std::size_t const prev = contents.rfind('\n', pos);
        std::size_t const from = (prev == std::string::npos) ? 0 : prev + 1;
        std::size_t const next = contents.find('\n', pos);
        return contents.substr(from, next == std::string::npos ? next : next - from);
    };

    CHECK(line_with("probe_warning").find("[warning]") != std::string::npos);
    CHECK(line_with("probe_error").find("[error]") != std::string::npos);
    CHECK(line_with("probe_info").find("[info]") != std::string::npos);

    // Severity labels must not be borrowed between messages.
    CHECK(line_with("probe_warning").find("[error]") == std::string::npos);
    CHECK(line_with("probe_error").find("[warning]") == std::string::npos);
    CHECK(line_with("probe_info").find("[warning]") == std::string::npos);

    lm.setLevelFilter(prev_level);
    lm.setLogToFile(prev_file);
    lm.clearVisibleModules();
    fs::remove_all(log_dir, ec);
#endif
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
