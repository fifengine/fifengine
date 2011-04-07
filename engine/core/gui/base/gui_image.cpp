/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include "util/log/logger.h"
#include "video/image.h"
#include "video/imagemanager.h"

#include "gui_image.h"

namespace FIFE {
	static Logger _log(LM_GUI);

	GuiImage::GuiImage(): gcn::Image() {
	}

	GuiImage::GuiImage(int32_t id = 0): gcn::Image(), m_poolid(id) {
//prock - 504
//TODO: this store a ImagePtr
	}

	GuiImage::~GuiImage() {
 		//ImageManager::instance()->free(m_poolid);
	}

	void GuiImage::free() {
		//ImageManager::instance()->free(m_poolid);
	}

	int32_t GuiImage::getWidth() const {
//prock - 504
		if(m_poolid==0)
			return 0;
		return ImageManager::instance()->get(m_poolid)->getWidth();
	}

	int32_t GuiImage::getHeight() const {
//prock - 504
		if(m_poolid==0)
			return 0;
		return ImageManager::instance()->get(m_poolid)->getHeight();
	}

	gcn::Color GuiImage::getPixel(int32_t x, int32_t y) {
		FL_PANIC(_log, "GuiImage::getPixel, not implemented");
		return gcn::Color();
	}

	void GuiImage::putPixel(int32_t x, int32_t y, const gcn::Color& color) {
		FL_PANIC(_log, "GuiImage::putPixel, not implemented");
	}

	void GuiImage::convertToDisplayFormat() {
		// empty on purpose
		// Since we don't need any conversions - this is just a proxy.
	}
}
