/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/atlasbook.h"
#include "video/renderbackend.h"

#include "gui_imageloader.h"

static const uint32_t ATLAS_SIZE = 512;

namespace FIFE {
	GuiImageLoader::GuiImageLoader() {
		m_atlasbook = new AtlasBook(ATLAS_SIZE, ATLAS_SIZE);
	}

	GuiImageLoader::~GuiImageLoader() {
		delete m_atlasbook;
	}

	fcn::Image* GuiImageLoader::load(const std::string& filename, bool convertToDisplayFormat) {
		ImageManager* imgManager = ImageManager::instance();

		if(imgManager->exists(filename)) {
			return new GuiImage(imgManager->get(filename));
		}
		// load demanded image
		ImagePtr tmpimg = imgManager->load(filename);
		if(tmpimg->getWidth() >= ATLAS_SIZE || tmpimg->getHeight() >= ATLAS_SIZE) {
			return new GuiImage(tmpimg);
		}
		// look for a place for an image of given size
		AtlasBlock* block = m_atlasbook->getBlock(tmpimg->getWidth(), tmpimg->getHeight());

		// if it can't fit, we need to add new 'page'
		if(block->page >= m_atlases.size()) {
			m_atlases.push_back(imgManager->loadBlank(ATLAS_SIZE, ATLAS_SIZE));

			// because we gonna update texture on-the fly (via TexSubImage)
			// we cant really use compressed texture 
			RenderBackend* rb = RenderBackend::instance();
			bool prev = rb->isImageCompressingEnabled();
			rb->setImageCompressingEnabled(false);
			m_atlases[block->page]->forceLoadInternal();
			rb->setImageCompressingEnabled(prev);
		}
		
		// update atlas page with given image
		m_atlases[block->page]->copySubimage(block->left, block->top, tmpimg);
	
		// we dont really need this image anymore 
		tmpimg->free();
		imgManager->remove(tmpimg);

		// create shared image and return it
		ImagePtr img = imgManager->create(filename);
		Rect region(block->left, block->top, block->getWidth(), block->getHeight());
		img->useSharedImage(m_atlases[block->page], region);

		return new GuiImage(img);
	}
}
