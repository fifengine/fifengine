/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

// Standard C++ library includes
#include <algorithm>
#include <iomanip>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
// #include "guichan_addon/console.h"
#include "util/log.h"
#include "util/debugutils.h"
#include "util/settingsmanager.h"

namespace FIFE {
	std::map<int, int> FlowTracer::thread2depth;

	Log::type_dbgtypes Log::m_dbgtypes;
	bool Log::m_showall = false;
	int Log::m_logLevelTreshold = 0;
// 	Console* Log::m_console = 0;

	Log::Log(const std::string& type, const type_log_level& log_level )
	: m_show(Log::m_showall || isVisibleLogLevel(log_level) || isVisibleType(type)) {
		if (m_show) {
			std::ostringstream stream;
			stream << std::setfill('0') << std::setw(8) << SDL_GetTicks() << " " << type << ": ";
			m_logstring = stream.str();
		};
	}

	Log::~Log() {
		if (m_show) {
			logString(m_logstring + "\n");
		}
	}

	void Log::parseCmdLine(const std::map<std::string,std::vector<std::string> >& args) {
		initialize();

		std::map<std::string, std::vector<std::string> >::const_iterator i(args.find("-debug"));
		if( i == args.end() )
			return;

		std::vector<std::string>::const_iterator j(i->second.begin());
		for(;j!=i->second.end(); ++j)
			Log::addVisibleType(*j);
	}

	void Log::initialize() {
		int llt = SettingsManager::instance()->read<int>("LogLevel",int(LEVEL_LOG));
		if (llt < 0 || llt > int(LEVEL_MAX)) {
			DEBUG_PRINT("log level from settings file is " << llt);
			llt= std::min(std::max(0,llt),int(LEVEL_MAX));
			SettingsManager::instance()->write<int>("LogLevel",llt);
		};
		m_logLevelTreshold = llt;
		m_showall = (llt == int(LEVEL_MAX));
	};

	bool Log::isVisibleLogLevel(const type_log_level& log_level ) {
		return m_logLevelTreshold > log_level;
	};

	void Log::setLogLevel(type_log_level log_level ) {
		if (int(log_level) < 0 || int(log_level) > int(LEVEL_MAX)) {
			DEBUG_PRINT("setting log level to: " << int(log_level) << " not possible");
			log_level = Log::type_log_level(std::min(std::max(0,int(log_level)),int(LEVEL_MAX)));
		};
		m_logLevelTreshold = log_level;
		m_showall = ( log_level == LEVEL_MAX );
	};
	void Log::addVisibleType(const std::string& type) {
		if (isVisibleType(type)) {
			return;
		}
		m_dbgtypes.insert(type);
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
		return m_dbgtypes.count(type);
	}

// 	void Log::setConsole(Console* console){
// 		m_console = console;
// 	};

	void Log::logString(const std::string& str){
		std::cout << str << std::flush;
// 		if( m_console )
// 			m_console->print( str );
	};

	FlowTracer::FlowTracer(const std::string &txt) {
		m_txt = txt;
		int threadId = SDL_ThreadID();
		std::cout << std::setw(FlowTracer::thread2depth[threadId] + 3) << "-> " << m_txt << std::endl;
		FlowTracer::thread2depth[threadId] += 2;
	}
	FlowTracer::~FlowTracer() {
		int threadId = SDL_ThreadID();
		FlowTracer::thread2depth[threadId] -= 2;
		std::cout << std::setw(FlowTracer::thread2depth[threadId] + 3) << "<- " << m_txt << std::endl;
		if (FlowTracer::thread2depth[threadId] < 0) {
			FlowTracer::thread2depth[threadId] = 0;
		}
	}	

}
