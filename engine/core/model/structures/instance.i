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
			Object* getObject() { return m_object; }
			void setPosition(const Point& p) { m_location.position = p; }
			const Point& getPosition() const { return m_location.position; }
			Layer* getLayer() const { return m_location.layer; }
			const Location& getLocation() const { return m_location; }
			void addListener(InstanceListener* listener);
			void removeListener(InstanceListener* listener);
			Action* getCurrentAction();
			float getMovementSpeed();
			std::vector<Location>* getNextCells();
			Point getOffsetSource();
			Point getOffsetTarget();
			double getOffsetDistance();
			Point getStaticDirection();
			int getOrientation() const;
			int getActionRuntime();
			void act(const std::string& action_name, const Location& target, const double speed);
			void act(const std::string& action_name, const Point& direction, bool repeating=false);
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
