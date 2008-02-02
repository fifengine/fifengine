%module fife
%{
#include "view/renderers/cellselectionrenderer.h"
%}

namespace FIFE {
	class RenderBackend;
	class View;

	class CellSelectionRenderer: public RendererBase {
	public:
		virtual ~CellSelectionRenderer();
		std::string getName();
		static CellSelectionRenderer* getInstance(View* view);
		void selectLocation(Location* loc);
		
	private:
		CellSelectionRenderer(RenderBackend* renderbackend, int position);
	};

}

