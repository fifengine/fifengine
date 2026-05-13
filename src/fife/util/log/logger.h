// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_LOGGER_H
#define FIFE_LOGGER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// 3rd party library includes
#ifdef LOG_ENABLED
    #include <spdlog/common.h>
    #include <spdlog/logger.h>
    #include <spdlog/sinks/sink.h>
#endif

// FIFE includes
#include "modules.h"
#include "util/base/fife_stdint.h"

/**
 * @def   FL_DBG
 *  Logs a debug-level message.
 * @param logger Logger instance (e.g. the static @c _log declared per .cpp file).
 * @param msg    Message to log (string literal, std::string, or std::format result).
 *
 * This macro first checks whether the logger's module is visible via
 * LogManager::isVisible(). If so, it forwards to Logger::log() with level
 * LogManager::LEVEL_DEBUG.
 */
#ifdef LOG_ENABLED
    #define FL_DBG(logger, msg) /* NOLINT(cppcoreguidelines-macro-usage,cppcoreguidelines-avoid-do-while) */ \
        do {                                                                                                 \
            if (FIFE::LogManager::instance()->isVisible((logger).getModule()))                               \
                (logger).log(FIFE::LogManager::LEVEL_DEBUG, (msg));                                          \
        } while (0)
#else
    #define FL_DBG(logger, msg)
#endif

/**
 * @def   FL_LOG
 *  Logs a standard log-level message.
 * @param logger Logger instance.
 * @param msg    Message to log.
 *
 * @see FL_DBG
 */
#ifdef LOG_ENABLED
    #define FL_LOG(logger, msg) /* NOLINT(cppcoreguidelines-macro-usage,cppcoreguidelines-avoid-do-while) */ \
        do {                                                                                                 \
            if (FIFE::LogManager::instance()->isVisible((logger).getModule()))                               \
                (logger).log(FIFE::LogManager::LEVEL_LOG, (msg));                                            \
        } while (0)
#else
    #define FL_LOG(logger, msg)
#endif

/**
 * @def   FL_WARN
 *  Logs a warning-level message.
 * @param logger Logger instance.
 * @param msg    Message to log.
 *
 * @see FL_DBG
 */
#ifdef LOG_ENABLED
    #define FL_WARN(logger, msg) /* NOLINT(cppcoreguidelines-macro-usage,cppcoreguidelines-avoid-do-while) */ \
        do {                                                                                                  \
            if (FIFE::LogManager::instance()->isVisible((logger).getModule()))                                \
                (logger).log(FIFE::LogManager::LEVEL_WARN, (msg));                                            \
        } while (0)
#else
    #define FL_WARN(logger, msg)
#endif

/**
 * @def   FL_ERR
 *  Logs an error-level message.
 * @param logger Logger instance.
 * @param msg    Message to log.
 *
 * @see FL_DBG
 */
#ifdef LOG_ENABLED
    #define FL_ERR(logger, msg) /* NOLINT(cppcoreguidelines-macro-usage,cppcoreguidelines-avoid-do-while) */ \
        do {                                                                                                 \
            if (FIFE::LogManager::instance()->isVisible((logger).getModule()))                               \
                (logger).log(FIFE::LogManager::LEVEL_ERROR, (msg));                                          \
        } while (0)
#else
    #define FL_ERR(logger, msg)
#endif

/**
 * @def   FL_PANIC
 *  Logs a panic-level message and aborts the program.
 * @param logger Logger instance.
 * @param msg    Message to log.
 *
 * After logging, the process is terminated via std::abort().
 * @see FL_DBG
 */
#ifdef LOG_ENABLED
    #define FL_PANIC(logger, msg) /* NOLINT(cppcoreguidelines-macro-usage,cppcoreguidelines-avoid-do-while) */ \
        do {                                                                                                   \
            if (FIFE::LogManager::instance()->isVisible((logger).getModule()))                                 \
                (logger).log(FIFE::LogManager::LEVEL_PANIC, (msg));                                            \
        } while (0)
#else
    #define FL_PANIC(logger, msg)
#endif

namespace FIFE
{

    /**
     *  Central logging controller.
     *
     * LogManager is a singleton that owns all log sinks (console, file) and
     * maintains the module-visibility bitmask.  Its primary responsibilities:
     *   - Filter messages by severity level (LogLevel).
     *   - Filter messages by module visibility (modules.h).
     *   - Route visible messages to spdlog sinks (color console, rotating file).
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
             * @return Pointer to the global LogManager.
             *
             * The instance is created on first call and intentionally leaked
             * to avoid static-destruction-order issues with the spdlog registry.
             */
            static LogManager* instance();

            /// Destructor (no-op – singleton is intentionally leaked).
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
             * The file sink writes to @c fife.log with a 5 MB size limit
             * and 3 rotated backups.
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
            LogManager();
            void validateModuleDescription(logmodule_t module);
            static bool isValidModule(logmodule_t module);

            LogLevel m_level;
            bool m_modules[LM_MODULE_MAX];

#ifdef LOG_ENABLED
            spdlog::logger* m_loggers[LM_MODULE_MAX];
            std::shared_ptr<spdlog::sinks::sink> m_console_sink;
            std::shared_ptr<spdlog::sinks::sink> m_file_sink;
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

} // namespace FIFE

#endif
