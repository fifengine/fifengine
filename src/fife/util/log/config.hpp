// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_LOG_CONFIG_HPP
#define FIFE_LOG_CONFIG_HPP

#include <cstddef>
#include <string>

#ifdef LOG_ENABLED
    #include <spdlog/common.h>
#endif

namespace FIFE
{

    /**
     * Logging configuration for the LogManager.
     *
     * Controls which sinks are created, log file rotation parameters,
     * and the default severity level for the root logger.
     */
    struct LogConfig
    {
            bool console_enabled      = true;
            bool file_enabled         = false;
            std::string file_path     = "fife.log";
            std::size_t file_max_size = 5 * 1024 * 1024; // 5 MB
            int file_max_files        = 3;

#ifdef LOG_ENABLED
            spdlog::level::level_enum default_level = spdlog::level::info;
#else
            int default_level = 2;
#endif
    };

} // namespace FIFE

#endif
