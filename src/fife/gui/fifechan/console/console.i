// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "gui/fifechan/console/console.h"
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