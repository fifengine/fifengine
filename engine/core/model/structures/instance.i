%module objectinfo
%{
#include "model/structures/instance.h"
%}

%include "util/point.h"

namespace FIFE { namespace model {

	class Instance {
		public:
			~Instance();

			void setPosition(const Point& p);
			const Point& getPosition() const;

		private:
			Instance();
	};
}}
