/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "log.h"
#include <algorithm>
#include <iomanip>
#include <SDL.h>
// #include "console.h"
#include "debugutils.h"
#include "settingsmanager.h"

namespace FIFE {

	Log::type_dbgtypes Log::m_dbgtypes;
	bool Log::m_showall = false;
	int Log::m_logLevelTreshold = 0;
// 	Console* Log::m_console = 0;

	Log::Log(const std::string& type, const type_log_level& log_level )
	: m_show(Log::m_showall || isVisibleLogLevel(log_level) || isVisibleType(type)) {
		if (m_show){
			std::ostringstream stream;
			stream << std::setfill('0') << std::setw(8) << SDL_GetTicks() << " " << type << ": ";
			std::string s = stream.str();
			m_logstring = s;
		};
	}

	Log::~Log() {
		if (m_show)
			logString(m_logstring + "\n");
	}

	void Log::parseCmdLine(int argc, char* argv[]) {
		bool debugargs = false;
		initialize();

		for (int i = 1; i < argc; ++i) {
			std::string arg = argv[i];
			if (debugargs)
				addVisibleType(arg);
			else {
				if (arg == "--debug") {
					debugargs = true;
					m_showall = false;
				}
			}
		}
	}

	void Log::initialize() {
		int llt = SettingsManager::instance()->read<int>("LogLevel",int(LEVEL_LOG));
		if( llt < 0 || llt > int(LEVEL_MAX) ) {
			DEBUG_PRINT("log level from settings file is " << llt);
			llt= std::min(std::max(0,llt),int(LEVEL_MAX));
			SettingsManager::instance()->write<int>("LogLevel",llt);
		};
		m_logLevelTreshold = llt;
		m_showall = ( llt == int(LEVEL_MAX) );
	};

	bool Log::isVisibleLogLevel(const type_log_level& log_level ) {
		return m_logLevelTreshold > log_level;
	};

	void Log::setLogLevel(type_log_level log_level ) {
		if( int(log_level) < 0 || int(log_level) > int(LEVEL_MAX) ) {
			DEBUG_PRINT("setting log level to: " << int(log_level) << " not possible");
			log_level = Log::type_log_level(std::min(std::max(0,int(log_level)),int(LEVEL_MAX)));
		};
		m_logLevelTreshold = log_level;
		m_showall = ( log_level == LEVEL_MAX );
	};
	void Log::addVisibleType(const std::string& type) {
		if (isVisibleType(type))
			return;
		m_dbgtypes.push_back(type);
	}
/*
	void Log::removeVisibleType(const std::string& type) {
		if (!isVisibleType(type))
			return;
		type_dbgtypes::iterator it = m_dbgtypes.begin();
		for(; it != m_dbgtypes.end(); ++it)
			if( *it == type ) {
				m_dbgtypes.erase( it );
				break;
			};
		if( isVisibleType(type) )
			 PANIC_PRINT("didnt remove " << type);
	}
*/
	bool Log::isVisibleType(const std::string& type) {
		return std::find(m_dbgtypes.begin(), m_dbgtypes.end(), type) != m_dbgtypes.end();
	}

// 	void Log::setConsole(Console* console){
// 		m_console = console;
// 	};

	void Log::logString(const std::string& str){
		std::cout << str << std::flush;
// 		if( m_console )
// 			m_console->print( str );
	};

}
