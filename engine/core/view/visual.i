%module fife
%{
#include "view/visual.h"
%}

namespace FIFE {
	class Visual2DGfx {
	public:
		virtual ~Visual2DGfx();
		void setTransparency(uint8_t transparency);
		unsigned int getTransparency();
		void setVisible(bool visible);
		unsigned int isVisible();
	private:
		Visual2DGfx();
	};

	%apply std::vector<int> &OUTPUT { std::vector<int>& angles };
	class ObjectVisual: public Visual2DGfx {
	public:
		static ObjectVisual* create(Object* object);
		virtual ~ObjectVisual();
		void addStaticImage(unsigned int angle, int image_index);
		int getStaticImageIndexByAngle(int angle);
		int getClosestMatchingAngle(int angle);
		void getStaticImageAngles(std::vector<int>& angles);
	private:
		ObjectVisual();
	};
	%template(get2dGfxVisual) Object::getVisual<ObjectVisual>;
	%clear std::vector<int> angles;
	
	class InstanceVisual: public Visual2DGfx {
	public:
		static InstanceVisual* create(Instance* instance);
		virtual ~InstanceVisual();
		void setStackPosition(int stackposition);
		int getStackPosition();
	private:
		InstanceVisual();
	};
	%template(get2dGfxVisual) Instance::getVisual<InstanceVisual>;
	
	class ActionVisual: public Visual2DGfx {
	public:
		static ActionVisual* create(Action* action);
		virtual ~ActionVisual();
		void addAnimation(unsigned int angle, int animation_index);
		int getAnimationIndexByAngle(int angle);
	private:
		ActionVisual();
	};
	%template(get2dGfxVisual) Action::getVisual<ActionVisual>;
}
