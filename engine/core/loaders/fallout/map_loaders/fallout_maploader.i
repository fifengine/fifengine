%module fallout_maploader
%{
#include "loaders/fallout/map_loaders/fallout.h"
%}

%include "model/map_loader.i"

namespace FIFE { namespace map { namespace loaders { namespace fallout {
	class Fallout : public MapLoader {
		public:
			Fallout();
			virtual ~Fallout();
			virtual MapPtr loadFile(const std::string& path);
			const std::string& getName() const;
	};
}}}}
