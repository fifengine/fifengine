%module native_maploader
%{
#include "loaders/native/map_loaders/xml.h"
%}

%include "map/map_loader.i"

namespace FIFE { namespace map { namespace loaders { namespace xml {
	class XML : public MapLoader {
		public:
			XML();
			~XML();
			virtual MapPtr loadFile(const std::string& path);
			const std::string& getName() const;
	};
}}}}
