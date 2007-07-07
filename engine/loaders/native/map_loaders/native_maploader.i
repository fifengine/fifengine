%module(directors="1") maploader
%{
#include "loaders/native/map_loaders/xml.h"
#include "map/map_loader.h"
%}

namespace FIFE { namespace map {

	class MapPtr;

	%feature("director") MapLoader;
	class MapLoader {
		public:
			MapLoader(const std::string& name);
			virtual ~MapLoader();
			virtual MapPtr loadFile(const std::string& path) = 0;
	};

namespace loaders { namespace xml {

	class XML : public MapLoader {
		public:
			XML();
			~XML();

			virtual MapPtr loadFile(const std::string& path);
      
		private:
	};
}}}}
