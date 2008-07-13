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

#ifndef FIFE_GUI_IMAGE_H
#define FIFE_GUI_IMAGE_H

// Standard C++ library includes

// 3rd party library includes
#include <guichan/color.hpp>
#include <guichan/image.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder


namespace FIFE {
	class ImagePool;

	class GuiImage : public gcn::Image {
	public:
		GuiImage();
		GuiImage(int id, ImagePool& pool);
		~GuiImage();
		void free();
		int getWidth() const;
		int getHeight() const;
		gcn::Color getPixel(int x, int y);
		void putPixel(int x, int y, const gcn::Color& color);
		void convertToDisplayFormat();
		int getPoolId() const { return m_poolid; }
	protected:
		int m_poolid;
		ImagePool* m_pool;
	};
}

#endif
