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
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

%module log
%{
#include "util/log.h"
%}

namespace FIFE {
	class Log {
	public:
		enum type_log_level {
			LEVEL_MAX   = 3,
			LEVEL_DEBUG = 2,
			LEVEL_LOG   = 1,
			LEVEL_WARN  = 0
		};
		Log(const std::string& type = "common", const type_log_level& logLevel = LEVEL_LOG );
		~Log();
		static void setLogLevel(type_log_level log_level);
		static void initialize(type_log_level loglevel, bool logtofile, bool logtoprompt);
	};
}
