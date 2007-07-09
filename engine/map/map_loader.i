%module(directors="1") maploader
%{
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
			const std::string& getName() const;
	};
}}