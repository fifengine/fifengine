%module guimanager
%{
#include "gui/guimanager.h"
%}

namespace gcn {
	class Widget;
}
namespace FIFE {
	class Console;
	class GUIManager {
	public:
		Console* getConsole();
		void add(gcn::Widget* widget);
		void remove(gcn::Widget* widget);
	};
}
