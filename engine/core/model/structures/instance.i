%module(directors="1") objectinfo
%{
#include "model/structures/instance.h"
%}

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
			std::vector<Location>* getNextLocations() const;
			const Location& getFacingCell() const;
			int getActionRuntime() const;
			void act(const std::string& action_name, const Location& target, const double speed);
			void act(const std::string& action_name, const Location& direction, bool repeating=false);
			int getStaticImageIndexByAngle(unsigned int angle);
			void update(unsigned int curticks=0);

			template<typename T>
			const T& get(const std::string& field);
			%template(get_bool) get<bool>;
			%template(get_int) get<long>;
			%template(get_Rect) get<Rect>;
			%template(get_Point) get<Point>;
			%template(get_string) get<std::string>;
	};

}
