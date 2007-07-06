%module maploader
%{
#include "loaders/native/map_loaders/xml.h"
#include "map/map_loader.h"
%}

namespace FIFE { namespace map {

	class MapPtr;

namespace loaders { namespace xml {

	class XML : public MapLoader {
		public:
			XML();
			~XML();

			virtual MapPtr loadFile(const std::string& path);
      
		private:
	};
}}}}
