%module mapstructure
%{
#include "map/structures/elevation.h"
%}

namespace FIFE { namespace map {

	class Elevation {
		public:
			~Elevation();

			size_t getNumLayers() const;

		private:
			Elevation();
	};
}}
