%module guimanager
%{
#include "gui/guimanager.h"
%}

namespace FIFE {
	class Console;
	class GUIManager {
	public:
		Console* getConsole();
	};
}
