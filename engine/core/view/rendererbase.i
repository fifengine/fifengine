%module view
%{
#include "view/rendererbase.h"
%}

namespace FIFE {
	class Camera;
	class Layer;
	class Instance;
	class Elevation;
	
	class RendererBase {
	public:
		RendererBase();
		virtual ~RendererBase();
		virtual std::string getName() = 0;
		int getPipelinePosition() { return m_position; }
		void setPipelinePosition(int position);
		void setEnabled(bool enabled);
		bool isEnabled();
		void addActiveLayer(Layer* layer);
		void removeActiveLayer(Layer* layer);
		void clearActiveLayers();
		void activateAllLayers(Elevation* elevation);
	};
}
