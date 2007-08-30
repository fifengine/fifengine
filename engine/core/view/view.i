%module view
%{
#include "view/view.h"
%}

namespace FIFE {
	class View {
	public:
		~View();
		void addCamera(Camera* camera);
		void removeCamera(Camera* camera);
	private:
		View(RenderBackend* renderbackend);	
	};
}
