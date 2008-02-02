%module fife
%{
#include "view/renderers/floatingtextrenderer.h"
%}

namespace FIFE {
	class RenderBackend;
	class AbstractFont;
	class View;

	class FloatingTextRenderer: public RendererBase {
	public:
		virtual ~FloatingTextRenderer();
		void changeDefaultFont(AbstractFont* font);
		static FloatingTextRenderer* getInstance(View* view);

	private:
		FloatingTextRenderer(RenderBackend* renderbackend, int position, AbstractFont* font);
	};


}
