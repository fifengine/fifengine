%module fife
%{
#include "model/structures/instance.h"
%}

%include "model/metamodel/modelcoords.i"
%include "model/metamodel/abstractvisual.i"
%include "util/point.i"
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

	class Instance {
	public:
		Instance(Object* object, const Location& location);
		virtual ~Instance();
		Object* getObject();
		void setLocation(const Location& loc);
		const Location& getLocation() const;
		const Location& getTargetLocation() const;
		void addListener(InstanceListener* listener);
		void removeListener(InstanceListener* listener);
		Action* getCurrentAction() const;
		double getMovementSpeed() const;
		const Location& getFacingLocation() const;
		int getActionRuntime() const;
		void act(const std::string& action_name, const Location& target, const double speed);
		void act_here(const std::string& action_name, const Location& direction, bool repeating=false);
		void update(unsigned int curticks=0);

		const std::string& get(const std::string& field);
		void setVisual(AbstractVisual* visual);
		template<typename T> T* getVisual() const;
	};
}
