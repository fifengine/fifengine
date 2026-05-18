// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_LOGGER_H
#define FIFE_LOGGER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

// 3rd party library includes
#ifdef LOG_ENABLED
    #include <spdlog/common.h>
    #include <spdlog/logger.h>
    #include <spdlog/sinks/dist_sink.h>
    #include <spdlog/sinks/sink.h>
#endif

// FIFE includes
#include "config.hpp"
#include "modules.h"
#include "util/base/fife_stdint.h"

namespace FIFE
{

#ifdef LOG_ENABLED
    inline constexpr bool kLogEnabled = true;
#else
    inline constexpr bool kLogEnabled = false;
#endif

    /**
     *  Central logging controller.
     *
     * LogManager is a singleton that owns all log sinks and
     * maintains the module-visibility bitmask.
     * It is configured via configure(LogConfig), which is safe to call even
     * after static Logger instances have already started using the defaults.
     * The file sink is created lazily only if logging is enabled (so we dont
     * get an empty fife.log when file logging is disabled).
     *
     * Its primary responsibilities:
     *   - Own the log sinks and per-module spdlog loggers.
     *   - Filter messages by module visibility (modules.h).
     *   - Route visible messages to spdlog sinks (color console, rotating file).
     *   - Lazily create a file sink, if enabled
     *
     * Use the FL_DBG / FL_LOG / FL_WARN / FL_ERR / FL_PANIC macros instead
     * of calling LogManager or Logger methods directly.
     */
    class FIFE_API LogManager
    {
        public:
            /**
             *  Log severity levels, in increasing priority.
             *
             * Messages below the active level filter are silently discarded.
             */
            enum LogLevel : uint8_t
            {
                LEVEL_DEBUG = 0, /**< Detailed debug information.           */
                LEVEL_LOG   = 1, /**< Standard informational message.       */
                LEVEL_WARN  = 2, /**< Warning – something unexpected.       */
                LEVEL_ERROR = 3, /**< Error – operation likely failed.       */
                LEVEL_PANIC = 4  /**< Fatal error – program aborts after log. */
            };

            /**
             *  Returns the singleton instance.
             * @return Reference to the global LogManager.
             *
             * The instance is a function-local static (Meyer's singleton),
             * initialized on first call and thread-safe in C++11+.
             * Its destructor is intentionally a no-op to avoid static
             * destruction-order issues with the spdlog registry.
             */
            static LogManager& instance();

            /**
             *  Reconfigures the LogManager with the given config.
             * @param cfg  The new logging configuration.
             *
             * Thread-safe: acquires an internal mutex before rebuilding sinks.
             * All existing per-module loggers are updated atomically via
             * spdlog::set_sinks().
             *
             * Safe to call after static Logger instances have already been
             * constructed — those loggers continue to point to their
             * per-module spdlog loggers, whose sinks are replaced in-place.
             */
            void configure(LogConfig const & cfg);

            /// Destructor (no-op – see instance()).
            ~LogManager();

            LogManager(LogManager const &)            = delete;
            LogManager& operator=(LogManager const &) = delete;

            /**
             *  Sets the minimum log level that will be output.
             * @param level  New severity threshold.
             *
             * Messages below @p level are silently discarded.
             * Example: setting LEVEL_WARN suppresses LEVEL_DEBUG and LEVEL_LOG.
             */
            void setLevelFilter(LogLevel level);

            /**
             *  Returns the current minimum log level.
             * @return Active LogLevel threshold.
             * @see setLevelFilter
             */
            LogLevel getLevelFilter();

            /**
             *  Makes a module (and its ancestors) visible.
             * @param module  Module to enable (e.g. LM_AUDIO, LM_VIEWVIEW).
             *
             * If the module has a parent, that parent is also made visible
             * recursively, ensuring the hierarchy is fully enabled.
             * By default all modules are invisible.
             */
            void addVisibleModule(logmodule_t module);

            /**
             *  Makes a module invisible.
             * @param module  Module to disable.
             * @see addVisibleModule
             */
            void removeVisibleModule(logmodule_t module);

            /**
             *  Hides all modules.
             * @see addVisibleModule
             */
            void clearVisibleModules();

            /**
             *  Checks whether a module (and its ancestors) is visible.
             * @param module  Module to query.
             * @return true if the module and all its ancestors are visible.
             *
             * Visibility propagates upward: if LM_VIEWVIEW is visible then
             * LM_VIEW and LM_CORE are implicitly visible.
             */
            bool isVisible(logmodule_t module);

            /**
             *  Enables or disables console output.
             * @param logtoprompt  true to enable the color console sink.
             *
             * This is a shorthand for configure() that toggles only the
             * console_enabled flag.
             */
            void setLogToPrompt(bool logtoprompt);

            /**
             *  Returns whether console output is enabled.
             * @return true if the console sink is active.
             */
            bool isLogToPrompt() const;

            /**
             *  Enables or disables file output.
             * @param logtofile  true to enable the rotating file sink.
             *
             * This is a shorthand for configure() that toggles only the
             * file_enabled flag.
             *
             * Unlike the legacy implementation, no fife.log file is created
             * unless this flag is (or has been) true.
             */
            void setLogToFile(bool logtofile);

            /**
             *  Returns whether file output is enabled.
             * @return true if the file sink is active.
             */
            bool isLogToFile() const;

            /**
             *  Returns the human-readable display name of a module.
             * @param module  Module identifier.
             * @return Display name (e.g. "Audio", "View::View").
             *
             * Returns "Unknown" for out-of-range module IDs.
             */
            char const * getModuleName(logmodule_t module);

#ifdef LOG_ENABLED
            /**
             *  Returns the spdlog logger for a module.
             * @param module  Module identifier.
             * @return Pointer to the spdlog::logger, or nullptr if invalid.
             *
             * @internal Used internally by the Logger class.
             */
            spdlog::logger* getSpdlogLogger(logmodule_t module);
#endif

        private:
            // Only instance() may construct a LogManager.
            LogManager();

            void validateModuleDescription(logmodule_t module);
            static bool isValidModule(logmodule_t module);

#ifdef LOG_ENABLED
            /** Rebuild all sinks and propagate them to every registered logger. */
            void rebuildSinks();
#endif

            std::mutex m_config_mutex;
            LogConfig m_config;

            LogLevel m_level;
            bool m_modules[LM_MODULE_MAX];

#ifdef LOG_ENABLED
            spdlog::logger* m_loggers[LM_MODULE_MAX];
            std::shared_ptr<spdlog::logger> m_root_logger;
            std::shared_ptr<spdlog::sinks::dist_sink_mt> m_dist_sink;
#endif
    };

    /**
     *  Per-module logger handle.
     *
     * Each .cpp file typically declares a file-static Logger at namespace
     * scope using the lambda-init pattern:
     *
     * @code
     * static Logger& _log = []() -> Logger& {
     *     static Logger log(LM_AUDIO);
     *     return log;
     * }();
     * @endcode
     *
     * The Logger stores its module identifier and a cached pointer to the
     * corresponding spdlog::logger.  The FL_* macros should be used for
     * all logging; direct calls to Logger::log are uncommon.
     */
    class FIFE_API Logger
    {
        public:
            /**
             *  Constructs a logger for the given module.
             * @param module  Module identifier (e.g. LM_CONTROLLER, LM_VIEW).
             *
             * The constructor looks up the spdlog logger from LogManager
             * by module ID.  This is a cheap O(1) array lookup.
             */
            explicit Logger(logmodule_t module);

            ~Logger() = default;

            Logger(Logger const &)            = default;
            Logger& operator=(Logger const &) = default;

            /**
             *  Logs a message at the given severity.
             * @param level  Severity level.
             * @param msg    The message string (or std::format result).
             *
             * When LOG_ENABLED is not defined this function is a no-op
             * and the compiler may elide the call entirely.
             * If @p level is LEVEL_PANIC the process calls std::abort()
             * after flushing the log.
             */
            void log(LogManager::LogLevel level, std::string const & msg);

            /**
             *  Returns the module associated with this logger.
             * @return The logmodule_t value passed at construction.
             */
            logmodule_t getModule() const
            {
                return m_module;
            }

        private:
            logmodule_t m_module;
#ifdef LOG_ENABLED
            spdlog::logger* m_logger;
#endif
    };

    /**
     * @def   FL_DBG
     *  Logs a debug-level message.
     * @param logger Logger instance.
     * @param msg    Message to log (string literal, std::string, or std::format result).
     *
     * These are inline template functions (not macros) that first check whether
     * the logger's module is visible via LogManager::isVisible() before logging.
     * When @c LOG_ENABLED is not defined the function body is eliminated by
     * @c if constexpr and the compiler may elide the call entirely.
     */
    template <typename Msg>
    inline void FL_DBG([[maybe_unused]] Logger& logger, [[maybe_unused]] Msg&& msg)
    {
        if constexpr (kLogEnabled) {
            if (LogManager::instance().isVisible(logger.getModule()))
                logger.log(LogManager::LEVEL_DEBUG, std::forward<Msg>(msg));
        }
    }

    template <typename Msg>
    inline void FL_LOG([[maybe_unused]] Logger& logger, [[maybe_unused]] Msg&& msg)
    {
        if constexpr (kLogEnabled) {
            if (LogManager::instance().isVisible(logger.getModule()))
                logger.log(LogManager::LEVEL_LOG, std::forward<Msg>(msg));
        }
    }

    template <typename Msg>
    inline void FL_WARN([[maybe_unused]] Logger& logger, [[maybe_unused]] Msg&& msg)
    {
        if constexpr (kLogEnabled) {
            if (LogManager::instance().isVisible(logger.getModule()))
                logger.log(LogManager::LEVEL_WARN, std::forward<Msg>(msg));
        }
    }

    template <typename Msg>
    inline void FL_ERR([[maybe_unused]] Logger& logger, [[maybe_unused]] Msg&& msg)
    {
        if constexpr (kLogEnabled) {
            if (LogManager::instance().isVisible(logger.getModule()))
                logger.log(LogManager::LEVEL_ERROR, std::forward<Msg>(msg));
        }
    }

    template <typename Msg>
    inline void FL_PANIC([[maybe_unused]] Logger& logger, [[maybe_unused]] Msg&& msg)
    {
        if constexpr (kLogEnabled) {
            if (LogManager::instance().isVisible(logger.getModule()))
                logger.log(LogManager::LEVEL_PANIC, std::forward<Msg>(msg));
        }
    }

} // namespace FIFE

#endif
