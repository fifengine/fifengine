/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderablepool.h"
#include "util/debugutils.h"

#include "gui_image.h"

namespace FIFE {
	GuiImage::GuiImage(): gcn::Image(), m_poolid(Pool::INVALID_ID), m_pool(NULL) {
	}

	GuiImage::GuiImage(int id, RenderablePool& pool): gcn::Image(), m_poolid(id), m_pool(&pool) {
	} 

	void GuiImage::free() {
		// the renderablepool should do this; should we tell it?
	}

	int GuiImage::getWidth() const {
		Renderable& img = dynamic_cast<Renderable&>(m_pool->get(m_poolid));
		return img.getWidth();
	}

	int GuiImage::getHeight() const {
		Renderable& img = dynamic_cast<Renderable&>(m_pool->get(m_poolid));
		return img.getWidth();
	}

	gcn::Color GuiImage::getPixel(int x, int y) {
		PANIC_PRINT("not implemented");
		return gcn::Color();
	}

	void GuiImage::putPixel(int x, int y, const gcn::Color& color) {
		PANIC_PRINT("not implemented");
	}

	void GuiImage::convertToDisplayFormat() {
		// empty on purpose
		// good idea?
	}
}
