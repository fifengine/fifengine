// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

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