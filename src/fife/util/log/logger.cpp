// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "logger.h"

// Standard C++ library includes
#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

// 3rd party library includes
#ifdef LOG_ENABLED
    #include <spdlog/common.h>
    #include <spdlog/sinks/dist_sink.h>
    #include <spdlog/sinks/null_sink.h>
    #include <spdlog/sinks/rotating_file_sink.h>
    #include <spdlog/sinks/stdout_color_sinks.h>
    #include <spdlog/spdlog.h>
#endif

// FIFE includes
#include "modules.h"
#include "util/base/exception.h"

// define the module info relationships structure here, begin
struct ModuleInfo
{
        logmodule_t module;
        logmodule_t parent;
        char const * displayName;
        char const * spdlogName;
};

static constexpr auto moduleInfos = std::to_array<ModuleInfo>(
    {{.module = LM_AUDIO, .parent = LM_CORE, .displayName = "Audio", .spdlogName = "Audio"},
     {.module = LM_CONTROLLER, .parent = LM_CORE, .displayName = "Controller", .spdlogName = "Controller"},
     {.module = LM_EVTCHANNEL, .parent = LM_CORE, .displayName = "Event Channel", .spdlogName = "EventChannel"},
     {.module = LM_GUI, .parent = LM_CORE, .displayName = "GUI", .spdlogName = "GUI"},
     {.module = LM_CONSOLE, .parent = LM_GUI, .displayName = "Console", .spdlogName = "GUI.Console"},
     {.module = LM_LOADERS, .parent = LM_CORE, .displayName = "Loaders", .spdlogName = "Loaders"},
     {.module      = LM_NATIVE_LOADERS,
      .parent      = LM_LOADERS,
      .displayName = "Native loaders",
      .spdlogName  = "Loaders.Native"},
     {.module = LM_FO_LOADERS, .parent = LM_LOADERS, .displayName = "Fallout loaders", .spdlogName = "Loaders.Fallout"},
     {.module = LM_SAVERS, .parent = LM_CORE, .displayName = "Savers", .spdlogName = "Savers"},
     {.module = LM_NATIVE_SAVERS, .parent = LM_CORE, .displayName = "Native savers", .spdlogName = "Savers.Native"},
     {.module = LM_MODEL, .parent = LM_CORE, .displayName = "Model", .spdlogName = "Model"},
     {.module = LM_STRUCTURES, .parent = LM_MODEL, .displayName = "Structures", .spdlogName = "Model.Structures"},
     {.module      = LM_INSTANCE,
      .parent      = LM_STRUCTURES,
      .displayName = "Instance",
      .spdlogName  = "Model.Structures.Instance"},
     {.module      = LM_LOCATION,
      .parent      = LM_STRUCTURES,
      .displayName = "Location",
      .spdlogName  = "Model.Structures.Location"},
     {.module = LM_METAMODEL, .parent = LM_MODEL, .displayName = "Metamodel", .spdlogName = "Model.Metamodel"},
     {.module      = LM_CELLGRID,
      .parent      = LM_METAMODEL,
      .displayName = "Cellgrid",
      .spdlogName  = "Model.Metamodel.CellGrid"},
     {.module      = LM_SQUAREGRID,
      .parent      = LM_METAMODEL,
      .displayName = "Squaregrid",
      .spdlogName  = "Model.Metamodel.SquareGrid"},
     {.module = LM_HEXGRID, .parent = LM_METAMODEL, .displayName = "Hexgrid", .spdlogName = "Model.Metamodel.HexGrid"},
     {.module = LM_PATHFINDER, .parent = LM_CORE, .displayName = "Pathfinder", .spdlogName = "PathFinder"},
     {.module = LM_UTIL, .parent = LM_CORE, .displayName = "Util", .spdlogName = "Util"},
     {.module = LM_RESMGR, .parent = LM_UTIL, .displayName = "Resource Manager", .spdlogName = "Util.ResourceManager"},
     {.module = LM_VFS, .parent = LM_CORE, .displayName = "VFS", .spdlogName = "VFS"},
     {.module = LM_VIDEO, .parent = LM_CORE, .displayName = "Video", .spdlogName = "Video"},
     {.module = LM_VIEW, .parent = LM_CORE, .displayName = "View", .spdlogName = "View"},
     {.module = LM_CAMERA, .parent = LM_VIEW, .displayName = "Camera", .spdlogName = "View.Camera"},
     {.module = LM_VIEWVIEW, .parent = LM_VIEW, .displayName = "View::View", .spdlogName = "View.ViewView"},
     {.module = LM_XML, .parent = LM_CORE, .displayName = "XML", .spdlogName = "XML"},
     {.module = LM_EXCEPTION, .parent = LM_CORE, .displayName = "Exception", .spdlogName = "Exception"},
     {.module = LM_SCRIPT, .parent = LM_CORE, .displayName = "Script", .spdlogName = "Script"},
     {.module = LM_CURSOR, .parent = LM_CORE, .displayName = "Cursor", .spdlogName = "Cursor"}});
// end

namespace FIFE
{

    Logger::Logger(logmodule_t module) :
        m_module(module)
#ifdef LOG_ENABLED
        ,
        m_logger(nullptr)
#endif
    {
#ifdef LOG_ENABLED
        m_logger = LogManager::instance().getSpdlogLogger(module);
#endif
    }

    void Logger::log(LogManager::LogLevel level, std::string const & msg)
    {
#ifdef LOG_ENABLED
        if (m_logger) {
            auto spd_level = static_cast<spdlog::level::level_enum>(level);
            m_logger->log(spd_level, msg);
            if (level == LogManager::LEVEL_PANIC) {
                m_logger->flush();
                abort();
            }
        }
#else
        (void)level;
        (void)msg;
#endif
    }

    LogManager& LogManager::instance()
    {
        static LogManager mgr;
        return mgr;
    }

    LogManager::~LogManager() = default;

    void LogManager::configure(LogConfig const & cfg)
    {
        std::scoped_lock const lock(m_config_mutex);
        m_config = cfg;
#ifdef LOG_ENABLED
        rebuildSinks();
#endif
    }

    void LogManager::setLevelFilter(LogLevel level)
    {
        m_level = level;
    }

    LogManager::LogLevel LogManager::getLevelFilter()
    {
        return m_level;
    }

    void LogManager::addVisibleModule(logmodule_t module)
    {
        if (!isValidModule(module)) {
            std::cout << "Invalid module received in LogManager: " << static_cast<int32_t>(module) << ", aborting\n";
            abort();
        }
        auto ind       = static_cast<size_t>(module);
        m_modules[ind] = true;
        if (moduleInfos[ind].parent != LM_CORE) {
            addVisibleModule(moduleInfos[ind].parent);
        }
    }

    void LogManager::removeVisibleModule(logmodule_t module)
    {
        if (!isValidModule(module)) {
            std::cout << "Invalid module received in LogManager: " << static_cast<int32_t>(module) << ", aborting\n";
            abort();
        }
        m_modules[static_cast<size_t>(module)] = false;
    }

    void LogManager::clearVisibleModules()
    {
        for (auto& m : m_modules) {
            m = false;
        }
    }

    void LogManager::setLogToPrompt(bool logtoprompt)
    {
        LogConfig cfg       = m_config;
        cfg.console_enabled = logtoprompt;
        configure(cfg);
    }

    bool LogManager::isLogToPrompt() const
    {
        return m_config.console_enabled;
    }

    void LogManager::setLogToFile(bool logtofile)
    {
        LogConfig cfg    = m_config;
        cfg.file_enabled = logtofile;
        configure(cfg);
    }

    bool LogManager::isLogToFile() const
    {
        return m_config.file_enabled;
    }

    bool LogManager::isVisible(logmodule_t module)
    {
        if (!isValidModule(module)) {
            return false;
        }
        auto ind = static_cast<size_t>(module);
        if (!m_modules[ind]) {
            return false;
        }
        if (moduleInfos[ind].parent != LM_CORE) {
            return isVisible(moduleInfos[ind].parent);
        }
        return true;
    }

    char const * LogManager::getModuleName(logmodule_t module)
    {
        if (!isValidModule(module)) {
            return "Unknown";
        }
        return moduleInfos[static_cast<size_t>(module)].displayName;
    }

    bool LogManager::isValidModule(logmodule_t module)
    {
        auto ind = static_cast<int32_t>(module);
        return ind > static_cast<int32_t>(LM_CORE) && ind < LM_MODULE_MAX;
    }

#ifdef LOG_ENABLED
    spdlog::logger* LogManager::getSpdlogLogger(logmodule_t module)
    {
        auto ind = static_cast<int32_t>(module);
        if (!isValidModule(module)) {
            return nullptr;
        }
        return m_loggers[ind];
    }
#endif

    void LogManager::validateModuleDescription(logmodule_t module)
    {
        if (module == LM_CORE) {
            for (size_t m = 0; m < static_cast<size_t>(LM_MODULE_MAX); m++) {
                if (moduleInfos[m].module != static_cast<logmodule_t>(m)) {
                    std::string msg = "Log module definition ids do not match in index ";
                    msg += std::to_string(m);
                    std::cout << msg << '\n';
                    throw InvalidFormat(msg);
                }
                validateModuleDescription(static_cast<logmodule_t>(m));
            }
        }
    }

    LogManager::LogManager() :
        m_config{},
        m_level(LEVEL_DEBUG),
        m_modules{}
#ifdef LOG_ENABLED
        ,
        m_loggers{},
        m_root_logger(nullptr),
        m_dist_sink(nullptr)
#endif
    {
        validateModuleDescription(LM_CORE);
        clearVisibleModules();

#ifdef LOG_ENABLED
        m_dist_sink = std::make_shared<spdlog::sinks::dist_sink_mt>();

        rebuildSinks();

        spdlog::flush_on(spdlog::level::warn);
#endif
    }

#ifdef LOG_ENABLED
    void LogManager::rebuildSinks()
    {
        std::vector<spdlog::sink_ptr> sinks;

        if (m_config.console_enabled) {
            sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        }

        if (m_config.file_enabled) {
            sinks.push_back(
                std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                    m_config.file_path, m_config.file_max_size, m_config.file_max_files));
        }

        if (sinks.empty()) {
            sinks.push_back(std::make_shared<spdlog::sinks::null_sink_mt>());
        }

        // Swap the dist_sink's children atomically.
        // All loggers (root + per-module) route through this single sink,
        // so changing it propagates to every logger instantly.
        m_dist_sink->set_sinks(std::move(sinks));

        // Create or update the root logger
        if (!m_root_logger) {
            m_root_logger = std::make_shared<spdlog::logger>("fife", m_dist_sink);
            m_root_logger->set_level(m_config.default_level);
            auto pattern = std::string("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
            m_root_logger->set_pattern(pattern);
        }

        // Create or update per-module loggers
        for (size_t i = 0; i < static_cast<size_t>(LM_MODULE_MAX); i++) {
            if (static_cast<int32_t>(i) <= static_cast<int32_t>(LM_CORE)) {
                continue;
            }

            auto module_name   = moduleInfos[i].spdlogName;
            auto module_logger = spdlog::get(module_name);

            if (!module_logger) {
                module_logger = std::make_shared<spdlog::logger>(module_name, m_dist_sink);
                module_logger->set_level(spdlog::level::trace);
                auto pattern = std::string("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
                module_logger->set_pattern(pattern);
                spdlog::register_logger(module_logger);
            }

            m_loggers[i] = module_logger.get();
        }
    }
#endif

} // namespace FIFE
