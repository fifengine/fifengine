#ifndef IANMAPFORMATFALLOUT_H
#define IANMAPFORMATFALLOUT_H

#include "maploader.h"
#include <map>
#include <string>
#include "vfs.h"

namespace FIFE {

	class RawData;
	class MFFalloutHeader;
	class PALAnimation;

	class MFFallout : public MapLoader {
		public:
			MFFallout();
			virtual ~MFFallout();

			virtual Map* loadFile(const std::string& path);

		private:
			MFFalloutHeader* m_header;

			struct s_imgdata {
				size_t cacheid;
				int shift_x;
				int shift_y;
				PALAnimation* pal_animation;
			};

			typedef std::map<std::string, s_imgdata> type_cached;
			type_cached m_cached;

			void loadTiles(Map* map, RawDataPtr data);
			void ignoreScripts(RawDataPtr data);
			void loadObjects(Map* map, RawDataPtr data);
			void generatePalAnimations(Map* map);
			void cleanup();

			const s_imgdata& getImageID(const std::string& path);

			MFFallout(const MFFallout&);
			MFFallout& operator=(const MFFallout&);
	};

}//FIFE

#endif
