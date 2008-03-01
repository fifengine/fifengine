%module fife
%{
#include "model/structures/instance.h"
%}

%include "model/metamodel/modelcoords.i"
%include "model/metamodel/abstractvisual.i"
%include "util/structures/utilstructures.i"
%include "location.i"

namespace FIFE {

	class Layer;
	class Action;
	class Object;
	class Instance;

	%feature("director") InstanceListener;
	class InstanceListener {
	public:
		virtual ~InstanceListener();
		virtual void OnActionFinished(Instance* instance, Action* action) = 0;
	};

	class Instance : public AttributedClass {
	public:
		Instance(Object* object, const Location& location, const std::string& identifier="");
		virtual ~Instance();
		Object* getObject();
		void setLocation(const Location& loc);
		Location getLocation() const;
		Location& getLocationRef();
		Location getTargetLocation() const;
		void addListener(InstanceListener* listener);
		void removeListener(InstanceListener* listener);
		Action* getCurrentAction() const;
		double getMovementSpeed() const;
		void setFacingLocation(const Location& loc);
		Location getFacingLocation() const;
		Location& getFacingLocationRef();
		int getActionRuntime() const;
		void move(const std::string& action_name, const Location& target, const double speed);
		void act(const std::string& action_name, const Location& direction, bool repeating=false);
		void follow(const std::string& action_name, Instance* leader, const double speed);
		void say(const std::string& text, unsigned int duration=0);
		void setTimeMultiplier(float multip);
		float getTimeMultiplier();
		void refresh();
		
		void setVisual(AbstractVisual* visual);
		template<typename T> T* getVisual() const;
	};
}

namespace std {
	%template(InstanceVector) vector<FIFE::Instance*>;
	%template(InstanceList) list<FIFE::Instance*>;	
}

