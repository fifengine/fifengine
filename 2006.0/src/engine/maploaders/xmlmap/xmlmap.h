#include "tinyxml/tinyxml.h"
#include "maploader.h"
#include "vfs.h"
#include "imagetool.h"

namespace FIFE {
	class RawData;

	class XMLMap : public MapLoader {
		public:
			XMLMap();
			virtual ~XMLMap();
			virtual Map* loadFile(const std::string& path);
		private:

			ImgcacheLoader icL;
			//void loadTiles();
			XMLMap(const XMLMap&);
			XMLMap& operator=(const XMLMap&);
			void cleanup();
			Map* loadTiles(Map*, TiXmlElement*);
			Map* loadElevation(Map*, TiXmlElement*);
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
