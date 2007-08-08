%module objectinfo
%{
#include "model/structures/instance.h"
%}

%include "util/point.h"
%include "location.h"

namespace FIFE { namespace model {

  class Layer;

  class Object;

	class Instance {
		public:

			Instance(Object* object, const Location& location);
			~Instance();

			template<typename T>
			const T& get(const std::string& field);
			%template(get_bool) get<bool>;
			%template(get_int) get<long>;
			%template(get_Rect) get<Rect>;
			%template(get_Point) get<Point>;
			%template(get_string) get<std::string>;

			void setPosition(const Point& p);

			const Point& getPosition() const;
			Layer* getLayer() const;
			const Location& getLocation() const;

		private:
	};
}}
