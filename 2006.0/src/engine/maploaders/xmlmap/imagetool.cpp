#include "SDL_image.h"
#include "imagetool.h"
#include "log.h"
#include "imagecache.h"

namespace FIFE {
	ImgIndexMap::~ImgIndexMap() {
		clear();
	}

	void ImgIndexMap::clear() {
		mapping.clear();
	}

	int ImgIndexMap::addPair(int tileId, size_t imgcacheId) {
		if (mapping.count(tileId)) {
			Log("ImageIndexMap") << "Error: Index " << tileId << " already registered!";
			return 1;
		}
		mapping[tileId] = imgcacheId;
		return 0;
	}

	size_t ImgIndexMap::getImageForId(int id) {
		std::map<int, size_t>::iterator idx = mapping.find(id);
		if (idx == mapping.end()) {
			Log("ImageIndexMap") << "Error: lookup for unknown id " << id;
			return 0;
		}
		else
			return idx->second;
	}

	// ImgcacheLoader

	ImgcacheLoader::ImgcacheLoader() {
		indices = new ImgIndexMap();
		sharedIndices = false;
	}

	ImgcacheLoader::ImgcacheLoader(ImgIndexMap* thisIndex) {
		indices = thisIndex;
		sharedIndices = true;
	}

	ImgcacheLoader::~ImgcacheLoader() {
		if (!sharedIndices) {
			delete indices;
		}
	}

	void ImgcacheLoader::clear() {
		indices->clear();
	}

	SDL_Surface* ImgcacheLoader::getSubImage(SDL_Surface* src, uint8_t w,
			uint8_t h, uint8_t xStep, uint8_t yStep) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		SDL_Surface* result = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
		SDL_Surface* result = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, w, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
		/* FIXME: this does not work! 
		 *
		 * Problem: the alpha of the src is not correctly blitted, can't make it work.
		 * the 'black' part of tiles overlaps others when drawn...
		 */
		/*
			 SDL_Rect src_rect;
			 src_rect.w = w;
			 src_rect.h = h;
			 src_rect.x = xStep*w;
			 src_rect.y = yStep*h;
			 SDL_Rect dst_rect;
			 dst_rect.x = 0;
			 dst_rect.y = 0;
			 Log("DEBUG") << src_rect.w << " " << src_rect.h << " " << src_rect.x << " " <<src_rect.y;

		// the next is required, otherwise the result is just BLACK
		SDL_FillRect(result, NULL, SDL_MapRGBA(result->format, 0, 0, 0, 255));

		if (SDL_BlitSurface(src, &src_rect, result, &dst_rect))
		Log("ImgcacheLoader") << "SDL_BlitSurface() failed!";*/
		//SDL_UpdateRects(result, 1, &dst_rect);


		/* better; still some strange lines at the border, not sure if it is the tileset
		 * or some off-by-one?
		 *
		 * Update: whatever, I fixed the (new) tiles; seems to work now.
		 */
		SDL_FillRect(result, NULL, SDL_MapRGBA(result->format, 0,0,0,255));

		SDL_LockSurface(result);
		uint32_t* pixeldest = static_cast<uint32_t*>(result->pixels);
		uint32_t* pixeldata = static_cast<uint32_t*>(src->pixels);
		pixeldata += src->w * yStep * h + xStep * w;

		for (uint16_t row = 0; row < h; row++) {
			for (uint16_t col = 0; col < w; col++) {
				*pixeldest = *pixeldata;
				++pixeldest;
				++pixeldata;
			}
			pixeldata += src->w - w - xStep * w;
			pixeldata += xStep * w;
		}
		SDL_UnlockSurface(result);
		return result;
	}

	int ImgcacheLoader::loadTileset(TiXmlElement* element) {
		// assume "element" is a valid node
		int tilewidth, tileheight = 0;
		int firstgid = 0;
		int rowskip = 0;
		const char* tilesetname = element->Attribute("name");
		if (!tilesetname) {
			Log("ImgcacheLoader") << "Warning: tileset with no name!";
			tilesetname = "[invalid]";
		}
		element->QueryIntAttribute("tilewidth", &tilewidth);
		element->QueryIntAttribute("tileheight", &tileheight);
		if (!tilewidth || !tileheight) {
			Log("ImgcacheLoader") << "Error: both tilewidth and tileheight attribute need to be defined!";
			return 1;
		}
		if (tilewidth > 255 || tileheight > 255) {
			Log("ImgcacheLoader") << "Error: tile sizes have to be <= 255";
			return 1;
		}

		TiXmlElement* imagenode = element->FirstChildElement("image");
		if (imagenode == NULL) {
			Log("ImgcacheLoader") << "Error: tileset needs to have at least one <image> child!";
			return 1;
		}

		ImageCache* ic = ImageCache::instance();
		while (imagenode != NULL) {
			const char* imgsrc = imagenode->Attribute("source");
			if (!imgsrc) {
				Log("ImgcacheLoader") << "Error: image node with no 'source' attribute!";
				return 1;
			}
			SDL_Surface* base = IMG_Load(imgsrc);
			if (!base) {
				Log("ImgcacheLoader") << "IMG_Load failed on " << imgsrc;
				return 1;
			}
			imagenode->QueryIntAttribute("firstgid", &firstgid);
			imagenode->QueryIntAttribute("rowskip", &rowskip);
			int xsteps = base->w / tilewidth;
			int ysteps = base->h / tileheight;
			for (uint8_t y = 0; y < static_cast<uint8_t>(ysteps); y++) {
				for (uint8_t x = 0; x < static_cast<uint8_t>(xsteps); x++) {
					SDL_Surface* subimg = getSubImage(base, static_cast<uint8_t>(tilewidth),
							static_cast<uint8_t>(tileheight), x, y);
					//char buff[100];
					//sprintf(reinterpret_cast<char*>(&buff), "test_%i.bmp", x+y*ysteps);
					//SDL_SaveBMP(subimg, reinterpret_cast<char*>(&buff));
					size_t iid = ic->addImage(subimg);
					if (!rowskip)
						Log("ImgcacheLoader") << "DEBUG1 " << firstgid+x+y*ysteps << " image " <<int(subimg->w) << "x"<<int(subimg->h) << " internal: " << iid;
					else
						Log("ImgcacheLoader") << "DEBUG2 " << firstgid+x+y*rowskip << " image " <<int(subimg->w) << "x"<<int(subimg->h) << " internal: " << iid;
					if (!rowskip)
						indices->addPair(firstgid+x+y*ysteps, iid);
					else
						indices->addPair(firstgid+x+y*rowskip, iid);
				}
			}
			imagenode = imagenode->NextSiblingElement("image");
		}
		return 0;
	}

	int ImgcacheLoader::loadImageAs(const char* filename, int id) {
		SDL_Surface* base = IMG_Load(filename);
		ImageCache* ic = ImageCache::instance();
		if (!base) {
			Log("ImgcacheLoader") << "IMG_Load failed on " << filename;
			return 1;
		}
		indices->addPair(id, ic->addImage(base));
		return 0;
	}
	size_t ImgcacheLoader::getImageForId(int id) {
		return indices->getImageForId(id);
	}

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
