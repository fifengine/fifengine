/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#include "imagetool.h"
#include "SDL_image.h"
#include "log.h"
#include "SDL_image.h"
#include "imagecache.h"
#include "vfs.h"
#include "rawdata.h"
#include "video/renderable.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"
#include "video/image.h"
#include "video/animation.h"

namespace FIFE {

	ImgcacheLoader::ImgcacheLoader() {
		m_imageIndexMap = type_imageIndexMapPtr(new type_imageIndexMap());
		m_isSharing = false;
	}

	ImgcacheLoader::ImgcacheLoader(type_imageIndexMapPtr thisIndex) {
		m_imageIndexMap = thisIndex;
		m_isSharing = true;
	}

	ImgcacheLoader::~ImgcacheLoader() {
	}

	ImgcacheLoader::type_imageIndexMapPtr ImgcacheLoader::getIndices() {
		if (!m_isSharing)
			Log("ImgcacheLoader") << "getIndices() called on an instance already using shared indices.";
		return m_imageIndexMap;
	}

	void ImgcacheLoader::clear() {
		m_imageIndexMap->clear();
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
 			//SDL_Surface* base = IMG_Load(imgsrc);
			SDL_Surface *base = loadImageAsSurface(imgsrc);
 			if (!base) {
 				Log("ImgcacheLoader") << "RenderableFactory failed on " << imgsrc;
 				return 1;
 			}
 			imagenode->QueryIntAttribute("firstgid", &firstgid);
 			imagenode->QueryIntAttribute("rowskip", &rowskip);
 			int xsteps = base->w / tilewidth;
 			int ysteps = base->h / tileheight;

			RenderBackend* rbackend = RenderManager::instance()->getCurrentBackend();
 			for (uint8_t y = 0; y < static_cast<uint8_t>(ysteps); y++) {
 				for (uint8_t x = 0; x < static_cast<uint8_t>(xsteps); x++) {
 					SDL_Surface* subimg = getSubImage(base, static_cast<uint8_t>(tilewidth),
 							static_cast<uint8_t>(tileheight), x, y);
 					//char buff[100];
 					//sprintf(reinterpret_cast<char*>(&buff), "test_%i.bmp", x+y*ysteps);
					//SDL_SaveBMP(subimg, reinterpret_cast<char*>(&buff));

					RenderAble* as_img = rbackend->createStaticImageFromSDL(subimg);
					size_t iid = ic->addImage(as_img);
 					if (!rowskip)
 						Log("ImgcacheLoader") << "DEBUG1 " << firstgid+x+y*ysteps << " image " <<int(as_img->getWidth()) << "x"<<int(as_img->getHeight()) << " internal: " << iid;
 					else
 						Log("ImgcacheLoader") << "DEBUG2 " << firstgid+x+y*rowskip << " image " <<int(as_img->getWidth()) << "x"<<int(as_img->getHeight()) << " internal: " << iid;
 					if (!rowskip)
 						(*m_imageIndexMap)[firstgid+x+y*ysteps] = iid;
 					else
 						(*m_imageIndexMap)[firstgid+x+y*rowskip] = iid;
 				}
 			}
 			imagenode = imagenode->NextSiblingElement("image");
 		}
		return 0;
	}

	int ImgcacheLoader::loadAnimation(TiXmlElement* element) {
		int frameDelay = 0;
		int actionFrame = 0;
		int id = 0;
		element->QueryIntAttribute("delay", &frameDelay);
		element->QueryIntAttribute("action", &actionFrame);
		element->QueryIntAttribute("id", &id);
		if (!id) {
			Log("ImgcacheLoader") << "Error: animation without id!";
			return 1;
		}

		std::vector<Image*> frames;
		
		TiXmlElement* subel = element->FirstChildElement("frame");
		if (!subel) {
			Log("ImgcacheLoader") << "Error: animation with no frame children!";
			return 1;
		}
		RenderBackend* rbackend = RenderManager::instance()->getCurrentBackend();
		
		while (subel != NULL) {
			const char* imgsrc = subel->Attribute("source");
 			if (!imgsrc) {
 				Log("ImgcacheLoader") << "Error: frame node with no 'source' attribute!";
 				continue;
 			}
 			//SDL_Surface* base = IMG_Load(imgsrc);
			SDL_Surface *base = loadImageAsSurface(imgsrc);
			if (!base) {
				Log("ImgcacheLoader") << "Error: could not load frame image: " << imgsrc;
				continue;
			}
			Image* as_image = rbackend->createStaticImageFromSDL(base);
			int x_off = 0;
			int y_off = 0;
			subel->QueryIntAttribute("x_off", &x_off);
			subel->QueryIntAttribute("y_off", &y_off);

			as_image->setXShift(x_off);
			as_image->setYShift(y_off);
			
			frames.push_back(as_image);
			subel = subel->NextSiblingElement("frame");
		}

		Animation* anim = new Animation(frames.size());
		anim->setFrameDuration(frameDelay);
		anim->setActionFrame(actionFrame);
		for (size_t i=0 ; i < frames.size(); ++i) {
			anim->setFrame(i, frames[i]);
		}
		anim->setCurrentFrame(0);
		
		(*m_imageIndexMap)[id] = ImageCache::instance()->addImage(anim);

		return 0;
	}

	SDL_Surface * ImgcacheLoader::loadImageAsSurface(const char* filename) {
		SDL_RWops* rwops = createRWops( filename );
		SDL_Surface* base = IMG_Load_RW(rwops,1);
		if (!base) {
			Log("ImgcacheLoader") << "Error loading " << filename;
			return NULL;
		}
		return base;
	}

	int ImgcacheLoader::loadImageAs(const char* filename, int id) {

		ImageCache *ic = ImageCache::instance();
		(*m_imageIndexMap)[id] = ic->addImageFromFile(filename);
		return 0;
		/// Superfluous code still here, will not be called.
		//RenderAble *base = RenderableFactory::instance()->createRenderable(filename, RenderAble::RT_IMAGE);

		SDL_Surface* base = loadImageAsSurface(filename);
		if (!base) {
			Log("ImgcacheLoader") << "Error loading " << filename;
			return 1;
		}


		RenderBackend* rbackend = RenderManager::instance()->getCurrentBackend();
		RenderAble* re = rbackend->createStaticImageFromSDL(base);

		(*m_imageIndexMap)[id] = ic->addImage(re);
		return 0;
	}
	size_t ImgcacheLoader::getImageForId(int id) {
		return (*m_imageIndexMap)[id];
	}

	SDL_RWops* ImgcacheLoader::createRWops(const char *filename) {
		RawDataPtr data = VFS::instance()->open(filename);
		size_t size = data->getDataLength();
		uint8_t* filedata = new uint8_t[ size ];
		data->readInto(filedata,size);

		SDL_RWops* retval =  SDL_RWFromMem( filedata, size );
		return retval;
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
