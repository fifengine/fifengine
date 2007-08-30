%module model
%{
#include "model/metamodel/action.h"
%}

%include "util/attributedclass.i"

namespace FIFE {

	class Action : public AttributedClass {
		public:
			Action();
			virtual ~Action();
			int getAnimationIndexByAngle(unsigned int angle);
			void addAnimation(unsigned int angle, int animation_index);
	};
}
