%module guimanager
%{
#include <guichan.hpp>
#include "gui/guimanager.h"
%}

namespace gcn {
	class Widget;
	class ActionEvent;
	class ActionListener {
	public:
		virtual void action(const ActionEvent& actionEvent) = 0;
	};
}
namespace FIFE {
	class Console;
	%feature("notabstract") GUIManager;
	class GUIManager: public gcn::ActionListener {
	public:
		Console* getConsole();
		void add(gcn::Widget* widget);
		void remove(gcn::Widget* widget);
	private:
		GUIManager(IWidgetListener* widgetListener);
	};
}
