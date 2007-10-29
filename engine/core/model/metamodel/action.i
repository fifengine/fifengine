%module fife
%{
#include "model/metamodel/action.h"
%}

%include "util/attributedclass.i"
%include "model/metamodel/abstractvisual.i"

namespace FIFE {

	class Action : public AttributedClass {
	public:
		Action(const std::string& identifier);
		virtual ~Action();
		void setVisual(AbstractVisual* visual);
		template<typename T> T* getVisual() const;
	};
}
