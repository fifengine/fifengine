%module mapstructure
%{
#include "map/structures/map.h"
%}

namespace FIFE { namespace map {

	class Map {
		public:
			~Map();

			size_t getNumElevations() const;

		private:
			Map();
	};
}}
