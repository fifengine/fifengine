%module fife
%{
#include "view/view.h"
%}

namespace FIFE {
	class RendererBase;
	class View {
	public:
		~View();
		Camera* addCamera(Layer* layr,Rect viewport, ExactModelCoordinate emc);
		void removeCamera(Camera* camera);
		void clearCameras();
		RendererBase* getRenderer(const std::string& name);
		void resetRenderers();
	private:
		View(RenderBackend* renderbackend);
	};
}
