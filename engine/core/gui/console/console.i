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

%module fife
%{
#include "gui/console/console.h"
%}

namespace FIFE {
	%feature("director") ConsoleExecuter;
	class ConsoleExecuter {
	public:
		virtual ~ConsoleExecuter() {}
		virtual void onToolsClick() = 0;
		virtual std::string onConsoleCommand(const std::string& command) = 0;
	};

	class Console {
	public:
		void show();
		void hide();
		void clear();
		void toggleShowHide();
    void println( const std::string & s );
		void setConsoleExecuter(ConsoleExecuter* const consoleexec);
		void removeConsoleExecuter();
	};
}
