%module fife
%{
#include "model/metamodel/object.h"
%}

%include "util/base/attributedclass.i"
%include "model/metamodel/abstractvisual.i"

namespace FIFE {

	class Action;
	class AbstractPather;

	class Object : public AttributedClass {
	public:
		Object(const std::string& identifier, Object* inherited=NULL);
		~Object();

		const std::string& get(const std::string& field);

		Action* createAction(const std::string& identifier);
		Action* getAction(const std::string& identifier);

		void setPather(AbstractPather* pather);
		AbstractPather* getPather();

		Object* getInherited();
		void adoptVisual(AbstractVisual* visual);
		template<typename T> T* getVisual() const;
		
		void setBlocking(bool blocking);
		bool isBlocking();
		
		void setStatic(bool stat);
		bool isStatic();
	};
}
