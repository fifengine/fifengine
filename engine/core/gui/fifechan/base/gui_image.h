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

#ifndef FIFE_GUI_IMAGE_H
#define FIFE_GUI_IMAGE_H

// Standard C++ library includes

// 3rd party library includes
#include <fifechan/color.hpp>
#include <fifechan/image.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"
#include "video/image.h"
#include "video/imagemanager.h"

namespace FIFE {

	class GuiImage : public fcn::Image {
	public:
		GuiImage();
		GuiImage(ImagePtr img);
		~GuiImage();
		void free();
		int32_t getWidth() const;
		int32_t getHeight() const;
		fcn::Color getPixel(int32_t x, int32_t y);
		void putPixel(int32_t x, int32_t y, const fcn::Color& color);
		void convertToDisplayFormat();
		ImagePtr getFIFEImage() const { return m_imgPtr; }
	protected:
		ImagePtr m_imgPtr;
	};
}

#endif
