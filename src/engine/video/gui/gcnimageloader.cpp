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
#if GUICHAN_VERSION == 4

// Standard C++ library includes

// 3rd party library includes
#include <boost/lexical_cast.hpp>

// FIFE includes
#include "debugutils.h"
#include "imagecache.h"

#include "gcnimageloader.h"

namespace FIFE {

	GCNImageLoader::GCNImageLoader() : m_rid(ImageCache::UNDEFINED_ID) {

	}


	GCNImageLoader::~GCNImageLoader() {
		discard();
	}


	void GCNImageLoader::prepare(const std::string& filename) {
		discard();
		m_rid = ImageCache::instance()->addImageFromFile(filename);

		/* the following would enable guichan to specify a frame inside
		 * a FRM file (like RenderableLocation); I thought it would be
		 * required, but so far I am wrong...
		 *
		 * syntax:
		 *   filename
		 *   filename:frame_index
		 *   filename:frame_index#direction_index
		 *
		 */

		/*
		size_t last_3a = filename.rfind(":");
		if (last_3a == std::string::npos) {
			m_rid = ImageCache::instance()->addImageFromFile(filename);
			return;
		}
		size_t last_23 = filename.rfind("#");

		std::string tmpfile = filename.substr(0, last_3a);
		std::string frame_id_s = filename.substr(last_3a + 1,
				(last_23 == std::string::npos) ? filename.size() - 1 - last_3a : filename.size() - 1 - last_23 );
		std::string dir_id_s = (last_23 != std::string::npos) ? 
			filename.substr(last_23 + 1, filename.size() - 1 - last_23) : "";
		//DEBUG_PRINT(tmpfile);
		//DEBUG_PRINT(frame_id_s);
		//DEBUG_PRINT(dir_id_s);
		if (last_23 == std::string::npos)
			m_rid = ImageCache::instance()->addImageFromLocation( 
					RenderableLocation( tmpfile, boost::lexical_cast<unsigned int>(frame_id_s)) );
		else
			m_rid = ImageCache::instance()->addImageFromLocation( 
					RenderableLocation( tmpfile, boost::lexical_cast<unsigned int>(frame_id_s), 
						boost::lexical_cast<unsigned int>(dir_id_s)) );
		*/

	}

	void GCNImageLoader::free(gcn::Image*) {
		// The imagecache will free the data.
	}

	void* GCNImageLoader::finalize() {
		return reinterpret_cast<void*>(m_rid);
	}

	void GCNImageLoader::discard() {
	}

	int GCNImageLoader::getHeight() const {
		RenderAble* img = ImageCache::instance()->getImage(m_rid);
		assert(img);
		return img->getHeight();
	}

	int GCNImageLoader::getWidth() const {
		RenderAble* img = ImageCache::instance()->getImage(m_rid);
		assert(img);
		return img->getWidth();
	}

	// Seems like guichan only uses these functions for bitmapfonts (or never) - for now we'll ignore them.
	gcn::Color GCNImageLoader::getPixel(int x, int y) {
		PANIC_PRINT("not implemented");
		return gcn::Color();
	}

	void GCNImageLoader::putPixel(int x, int y, const gcn::Color& color) {
		PANIC_PRINT("not implemented");
	}

	void* GCNImageLoader::getRawData() {
		PANIC_PRINT("not implemented");
		return 0;
	}

}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
