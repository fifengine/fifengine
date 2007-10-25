%module view
%{
#include "view/view.h"
%}

namespace FIFE {
	class RendererBase;
	class View {
	public:
		~View();
		Camera* addCamera();
		void removeCamera(Camera* camera);
		RendererBase* getRenderer(const std::string& name);
		void resetRenderers();
	private:
		View(RenderBackend* renderbackend);
	};
}
