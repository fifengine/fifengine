%module fife
%{
#include <Rocket/Core.h>
#include "gui/librocket/librocketmanager.h"
#include "gui/guimanager.h"
%}

%include "gui/guimanager.i"

namespace Rocket {
	namespace Core {
		%nodefaultctor ElementDocument;
		class ElementDocument {
		public:
			void Show();
			void Hide();
		};
	}
}


namespace FIFE {

	%feature("notabstract") LibRocketManager;
	class LibRocketManager : public IGUIManager {
	public:
		
		LibRocketManager();
		virtual ~LibRocketManager();
		
		void init(const std::string& backend, int32_t screenWidth, int32_t	screenHeight);
		Rocket::Core::ElementDocument* loadDocument(const std::string&);
		void unloadDocument(Rocket::Core::ElementDocument* document);
	};
}