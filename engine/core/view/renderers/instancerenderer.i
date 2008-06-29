%module fife
%{
#include "view/renderers/instancerenderer.h"
%}

namespace FIFE {
	class Location;
	class RenderBackend;
	class ImagePool;
	class AnimationPool;

	class InstanceRenderer: public RendererBase {
	public:
		virtual ~InstanceRenderer();
		std::string getName();
		void addOutlined(Instance* instance, int r, int g, int b, int width);
		void removeOutlined(Instance* instance);
		void removeAllOutlines();
		void addColored(Instance* instance, int r, int g, int b);
		void removeColored(Instance* instance);
		void removeAllColored();
		static InstanceRenderer* getInstance(IRendererContainer* cnt);
	private:
		InstanceRenderer(RenderBackend* renderbackend, int position, ImagePool* imagepool, AnimationPool* animpool);
	};
}
