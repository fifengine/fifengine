%module objectinfo
%{
#include "model/structures/objectinfo.h"
%}

%include "util/point.h"

namespace FIFE { namespace map {

	class ObjectInfo {
		public:
			~ObjectInfo();

			void setPosition(const Point& p);
			const Point& getPosition() const;

		private:
			ObjectInfo();
	};
}}
