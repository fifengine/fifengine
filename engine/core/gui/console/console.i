%module(directors="1") console
%{
#include "gui/console/console.h"
%}

namespace FIFE {
	%feature("director") ConsoleExecuter;
	class ConsoleExecuter {
		public:
			/** Destructor
			 */
			virtual ~ConsoleExecuter() {}

			/** Called when console tools button is clicked
			 */
			virtual void onToolsClick() = 0;

			/** Called when user has typed command to console and pressed enter
			 * @return response from executer
			 */
			virtual std::string onConsoleCommand(const std::string& command) = 0;
	};

	class Console {
		public:
			void show();
			void hide();
			void clear();
			void toggleShowHide();
			void setConsoleExecuter(ConsoleExecuter* const consoleexec);
			void removeConsoleExecuter();
	};
}