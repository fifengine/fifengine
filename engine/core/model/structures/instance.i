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

			Instance(const Object* object, const Location& location);
			~Instance();

			const Object* getObject();

			void setPosition(const Point& p);

			const Point& getPosition() const;
			Layer* getLayer() const;
			const Location& getLocation() const;

		private:
	};
}}
