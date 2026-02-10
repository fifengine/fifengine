%module fife
%{
#include "gui/hybrid/hybridguimanager.h"
#include "gui/guimanager.h"
%}

%include "gui/guimanager.i"

namespace FIFE {

	%feature("notabstract") HybridGuiManager;
	class HybridGuiManager : public IGUIManager {
	public:
		
		HybridGuiManager();
		virtual ~HybridGuiManager();
		
		void addGuiManager(IGUIManager* guiManager);
		void removeGuiManager(IGUIManager* guiManager);
	};
}