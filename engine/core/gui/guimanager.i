%module guimanager
%{
#include <guichan.hpp>
#include "gui/guimanager.h"
%}

namespace gcn {
	class Widget;
}
namespace FIFE {
	class Console;
	class GUIManager: public gcn::ActionListener {
	public:
		Console* getConsole();
		void add(gcn::Widget* widget);
		void remove(gcn::Widget* widget);
	private:
		GUIManager(IWidgetListener* widgetListener);
	};
}
