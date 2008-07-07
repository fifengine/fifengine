%module fife
%{
#include "view/renderers/geometricrenderer.h"
%}

namespace FIFE {
	class RenderBackend;
	
	class GeometricRenderer: public RendererBase {
	public:
		virtual ~GeometricRenderer();
		std::string getName();
		void addLine(Point p1, Point p2, int r, int g, int b);
		void removeAllLines();
		static GeometricRenderer* getInstance(IRendererContainer* cnt);
	private:
		GeometricRenderer(RenderBackend* renderbackend, int position);
	};
}
