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
		CEGuiManager(uint32_t screenWidth, uint32_t screenHeight);
		virtual ~CEGuiManager();
	};
}