%module model
%{
#include "model/metamodel/object.h"
%}

%include "util/attributedclass.i"
%include "model/metamodel/abstractvisual.i"

namespace FIFE {

	class Action;
	class AbstractPather;

	class Object : public AttributedClass {
	public:
		Object(const std::string& identifier, Object* inherited=NULL);
		~Object();

		const std::string& get(const std::string& field);

		Action* addAction(const std::string& identifier);
		Action* getAction(const std::string& identifier);

		void setPather(AbstractPather* pather);
		AbstractPather* getPather();

		Object* getInherited();
		void setVisual(AbstractVisual* visual);
		template<typename T> T* getVisual() const;
	};
}
