%module objectinfo
%{
#include "model/structures/objectinfo.h"
%}

%include "util/point.h"

namespace FIFE { namespace model {

	class Object {
		public:
			~Object();

			void setPosition(const Point& p);
			const Point& getPosition() const;

		private:
			Object();
	};
}}
