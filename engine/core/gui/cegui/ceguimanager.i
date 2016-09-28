%module fife
%{
#include "gui/cegui/ceguimanager.h"
#include "gui/guimanager.h"
%}

%include "gui/guimanager.i"

namespace FIFE {

	%feature("notabstract") CEGuiManager;
	class CEGuiManager : public IGUIManager {
	public:
		CEGuiManager();
		virtual ~CEGuiManager();
	};
}
