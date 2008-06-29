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

	%feature("director") InstanceActionListener;
	class InstanceActionListener {
	public:
		virtual ~InstanceActionListener() {};
		virtual void onInstanceActionFinished(Instance* instance, Action* action) = 0;
	};

	enum InstanceChangeType {
		ICHANGE_NO_CHANGES = 0x0000,
		ICHANGE_LOC = 0x0001,
		ICHANGE_FACING_LOC = 0x0002,
		ICHANGE_SPEED = 0x0004,
		ICHANGE_ACTION = 0x0008,
		ICHANGE_TIME_MULTIPLIER = 0x0010,
		ICHANGE_SAYTEXT = 0x0020,
		ICHANGE_ROTATION = 0x0040,
	};
	typedef unsigned int InstanceChangeInfo;
	
	%feature("director") InstanceChangeListener;
	class InstanceChangeListener {
	public:
		virtual ~InstanceChangeListener() {};
		virtual void onInstanceChanged(Instance* instance, InstanceChangeInfo info) = 0;
	};

	class Instance : public ResourceClass {
	public:
		Instance(Object* object, const Location& location, const std::string& identifier="");
		virtual ~Instance();
		const std::string& getId();
		Object* getObject();
		void setLocation(const Location& loc);
		Location getLocation() const;
		Location& getLocationRef();
		Location getTargetLocation() const;
		void setRotation(int);
		int getRotation() const;
		void addActionListener(InstanceActionListener* listener);
		void removeActionListener(InstanceActionListener* listener);
		void addChangeListener(InstanceChangeListener* listener);
		void removeChangeListener(InstanceChangeListener* listener);
		Action* getCurrentAction() const;
		double getMovementSpeed() const;
		void setFacingLocation(const Location& loc);
		Location getFacingLocation();
		Location& getFacingLocationRef();
		int getActionRuntime() const;
		void move(const std::string& action_name, const Location& target, const double speed);
		void act(const std::string& action_name, const Location& direction, bool repeating=false);
		void follow(const std::string& action_name, Instance* leader, const double speed);
		void say(const std::string& text, unsigned int duration=0);
		void setTimeMultiplier(float multip);
		float getTimeMultiplier();
		void refresh();
		InstanceChangeInfo getChangeInfo();
		
		void setVisual(AbstractVisual* visual);
		template<typename T> T* getVisual() const;
	};
}

namespace std {
	%template(InstanceVector) vector<FIFE::Instance*>;
	%template(InstanceList) list<FIFE::Instance*>;	
}

