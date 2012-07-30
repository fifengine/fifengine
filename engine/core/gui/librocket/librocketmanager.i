%module fife
%{
#include "gui/librocket/librocketmanager.h"
#include "gui/guimanager.h"
%}

%include "gui/guimanager.i"

namespace FIFE {

	%feature("notabstract") LibRocketManager;
	class LibRocketManager : public IGUIManager {
	public:
		
		LibRocketManager();
		virtual ~LibRocketManager();
		
		void init(const std::string& backend, int32_t screenWidth, int32_t screenHeight);
		void loadFont(const std::string& filepath);
		
		void showDebugger() const;
		void hideDebugger() const;
	};
}