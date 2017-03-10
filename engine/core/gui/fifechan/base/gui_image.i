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

%module fife
%{
#include <fifechan/image.hpp>
#include "gui/fifechan/base/gui_image.h"
#include "video/image.h"
%}

%rename(GcnImage) fcn::Image;
namespace fcn {
	class Image
	{
	public:
		Image();
		virtual ~Image();
		static Image* load(const std::string& filename, bool convertToDisplayFormat = true);
		virtual int32_t getWidth() const = 0;
		virtual int32_t getHeight() const = 0;
	};
}

namespace FIFE {
	class GuiImage: public fcn::Image {
	public:
		GuiImage();
		GuiImage(FIFE::ImagePtr Image);
		int32_t getWidth() const;
		int32_t getHeight() const;
	};
}
