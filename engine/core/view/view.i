%module fife
%{
#include "view/view.h"
%}

namespace std {
  %template(CameraVector) std::vector<FIFE::Camera*>;
}

namespace FIFE {
	class RendererBase;
	class View {
	public:
		~View();
		Camera* addCamera(const std::string& id, Layer* layr,Rect viewport, ExactModelCoordinate emc);
		Camera* getCamera(const std::string& id);
		std::vector<Camera*>& getCameras();
		void removeCamera(Camera* camera);
		void clearCameras();
		RendererBase* getRenderer(const std::string& name);
		void resetRenderers();
	private:
		View(RenderBackend* renderbackend);
	};
}
