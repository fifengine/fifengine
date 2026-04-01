// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "logger.h"
#include "modules.h"
#include "util/base/exception.h"

// define the module info relationships structure here, begin
struct ModuleInfo
{
    logmodule_t module;
    logmodule_t parent;
    std::string name;
};

ModuleInfo moduleInfos[] = {
    {.module = LM_AUDIO, .parent = LM_CORE, .name = "Audio"},
    {.module = LM_CONTROLLER, .parent = LM_CORE, .name = "Controller"},
    {.module = LM_EVTCHANNEL, .parent = LM_CORE, .name = "Event Channel"},
    {.module = LM_GUI, .parent = LM_CORE, .name = "GUI"},
    {.module = LM_CONSOLE, .parent = LM_GUI, .name = "Console"},
    {.module = LM_LOADERS, .parent = LM_CORE, .name = "Loaders"},
    {.module = LM_NATIVE_LOADERS, .parent = LM_LOADERS, .name = "Native loaders"},
    {.module = LM_FO_LOADERS, .parent = LM_LOADERS, .name = "Fallout loaders"},
    {.module = LM_SAVERS, .parent = LM_CORE, .name = "Savers"},
    {.module = LM_NATIVE_SAVERS, .parent = LM_CORE, .name = "Native savers"},
    {.module = LM_MODEL, .parent = LM_CORE, .name = "Model"},
    {.module = LM_STRUCTURES, .parent = LM_MODEL, .name = "Structures"},
    {.module = LM_INSTANCE, .parent = LM_STRUCTURES, .name = "Instance"},
    {.module = LM_LOCATION, .parent = LM_STRUCTURES, .name = "Location"},
    {.module = LM_METAMODEL, .parent = LM_MODEL, .name = "Metamodel"},
    {.module = LM_CELLGRID, .parent = LM_METAMODEL, .name = "Cellgrid"},
    {.module = LM_SQUAREGRID, .parent = LM_METAMODEL, .name = "Squaregrid"},
    {.module = LM_HEXGRID, .parent = LM_METAMODEL, .name = "Hexgrid"},
    {.module = LM_PATHFINDER, .parent = LM_CORE, .name = "Pathfinder"},
    {.module = LM_UTIL, .parent = LM_CORE, .name = "Util"},
    {.module = LM_RESMGR, .parent = LM_UTIL, .name = "Resource Manager"},
    {.module = LM_VFS, .parent = LM_CORE, .name = "VFS"},
    {.module = LM_VIDEO, .parent = LM_CORE, .name = "Video"},
    {.module = LM_VIEW, .parent = LM_CORE, .name = "View"},
    {.module = LM_CAMERA, .parent = LM_VIEW, .name = "Camera"},
    {.module = LM_VIEWVIEW, .parent = LM_VIEW, .name = "View::View"},
    {.module = LM_XML, .parent = LM_CORE, .name = "XML"},
    {.module = LM_EXCEPTION, .parent = LM_CORE, .name = "Exception"},
    {.module = LM_SCRIPT, .parent = LM_CORE, .name = "Script"},
    {.module = LM_CURSOR, .parent = LM_CORE, .name = "Cursor"}};
// end

namespace FIFE
{
    LogManager* LogManager::m_instance = nullptr;

    Logger::Logger(logmodule_t module) : m_module(module) { }

    void Logger::log(LogManager::LogLevel level, const std::string& msg)
    {
        LogManager::instance()->log(level, m_module, msg);
    }

    void Logger::log(LogManager::LogLevel level, const LMsg& msg)
    {
        LogManager::instance()->log(level, m_module, msg.str);
    }

    LogManager* LogManager::instance()
    {
        if (m_instance == nullptr) {
            m_instance = new LogManager();
        }
        return m_instance;
    }

    LogManager::~LogManager()
    {
        delete m_instance;
    }

    void LogManager::log(LogLevel level, logmodule_t module, const std::string& msg)
    {
        if (level < m_level) {
            return;
        }
        if (!isVisible(module)) {
            return;
        }
        std::string lvlstr;
        switch (level) {
        case LEVEL_DEBUG:
            lvlstr = "DEBUG";
            break;

        case LEVEL_LOG:
            lvlstr = "LOG";
            break;

        case LEVEL_WARN:
            lvlstr = "WARN";
            break;

        case LEVEL_ERROR:
            lvlstr = "ERROR";
            break;

        case LEVEL_PANIC:
            lvlstr = "PANIC";
            break;

        default:
            lvlstr = "ERROR";
            break;
        }
        if (m_logtoprompt) {
            std::cout << moduleInfos[module].name << ":" << lvlstr << ":" << msg << '\n';
        }
        if (m_logtofile) {
            *m_logfile << moduleInfos[module].name << ":" << lvlstr << ":" << msg << '\n';
        }
        if (level == LEVEL_PANIC) {
            abort();
        }
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
        validateModule(module);
        auto ind       = static_cast<int32_t>(module);
        m_modules[ind] = true;
        if (moduleInfos[ind].parent != LM_CORE) {
            addVisibleModule(moduleInfos[ind].parent);
        }
    }

    void LogManager::removeVisibleModule(logmodule_t module)
    {
        validateModule(module);
        m_modules[module] = false;
    }

    void LogManager::clearVisibleModules()
    {
        for (bool& m_module : m_modules) {
            m_module = false;
        }
    }

    void LogManager::setLogToPrompt(bool logtoprompt)
    {
        m_logtoprompt = logtoprompt;
    }

    bool LogManager::isLogToPrompt()
    {
        return m_logtoprompt;
    }

    void LogManager::setLogToFile(bool logtofile)
    {
        if (logtofile) {
            m_logfile = new std::ofstream("fife.log");
        } else {
            delete m_logfile;
        }
        m_logtofile = logtofile;
    }

    bool LogManager::isLogToFile()
    {
        return m_logtofile;
    }

    bool LogManager::isVisible(logmodule_t module)
    {
        if (!m_modules[module]) {
            return false;
        }
        if (moduleInfos[module].parent != LM_CORE) {
            return isVisible(moduleInfos[module].parent);
        }
        return true;
    }

    LogManager::LogManager() :
        m_level(LEVEL_DEBUG), m_logfile(nullptr), m_modules{}, m_logtofile(false), m_logtoprompt(false)
    {
        validateModuleDescription(LM_CORE);
        clearVisibleModules();
    }

    void LogManager::validateModule(logmodule_t m)
    {
        if ((m <= LM_CORE) || (m >= LM_MODULE_MAX)) {
            std::cout << "Invalid module received in LogManager: " << m << ", aborting\n";
            abort();
        }
    }

    void LogManager::validateModuleDescription(logmodule_t module)
    {
        if (module == LM_CORE) {
            for (int32_t m = static_cast<int32_t>(LM_CORE) + 1; m < static_cast<int32_t>(LM_MODULE_MAX); m++) {
                if (moduleInfos[m].module != static_cast<logmodule_t>(m)) {
                    std::ostringstream stream;
                    stream << m;
                    std::string msg = "Log module definition ids do not match in index ";
                    msg += stream.str();
                    std::cout << msg << '\n';
                    throw InvalidFormat(msg);
                }
                module_check_stack.clear();
                validateModuleDescription(static_cast<logmodule_t>(m));
            }
        } else {
            module_check_stack.push_back(module);
            if (count(module_check_stack.begin(), module_check_stack.end(), module) > 1) {
                throw InvalidFormat("Log module definition hierarchy contains cycles");
            }
        }
    }

    const std::string& LogManager::getModuleName(logmodule_t module)
    {
        return moduleInfos[module].name;
    }
} // namespace FIFE
