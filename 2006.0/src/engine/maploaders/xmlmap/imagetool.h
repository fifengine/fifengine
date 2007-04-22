#ifndef SDL_IMAGEHELPER_H
#define SDL_IMAGEHELPER_H
#include <sys/types.h>
#include <stdint.h>
#include <map>
#include <SDL.h>
#include "tinyxml/tinyxml.h"

namespace FIFE {
	class ImgIndexMap {
		private:
			std::map<int, size_t> mapping;
		public:
			~ImgIndexMap();

			void   clear();
			int    addPair(int tileId, size_t imgcacheId);
			size_t getImageForId(int);
	};
	class ImgcacheLoader {
		private:
			ImgIndexMap* indices;
			bool sharedIndices;
			SDL_Surface* getSubImage(SDL_Surface* src, uint8_t w, uint8_t h, uint8_t xStep, uint8_t yStep);
		public:
			ImgcacheLoader();
			ImgcacheLoader(ImgIndexMap* thisIndex);
			~ImgcacheLoader();

			int loadTileset(TiXmlElement* element);
			int loadImageAs(const char* filename, int id);
			size_t getImageForId(int);
			void clear();
	};
}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
