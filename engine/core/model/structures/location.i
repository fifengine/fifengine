#include "util/point.i"

namespace FIFE {

	class Elevation;
	class Layer;

	class Location {
		public:
			Location();

			Elevation* elevation;
			Layer* layer;
			Point position;

			bool operator==(const Location& loc) const;
	};

}
