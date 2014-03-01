%module fife
%{
#include "gui/cegui-0/cegui-0manager.h"
#include "gui/guimanager.h"
%}

%include "gui/guimanager.i"

namespace FIFE {

	%feature("notabstract") CEGui_0Manager;
	class CEGui_0Manager : public IGUIManager {
	public:
		CEGui_0Manager();
		virtual ~CEGui_0Manager();
	};
}
