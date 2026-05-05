// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GUI_IMAGE_H
#define FIFE_GUI_IMAGE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes
#include <fifechan/color.hpp>
#include <fifechan/image.hpp>

// FIFE includes
#include "util/base/fife_stdint.h"
#include "video/image.h"
#include "video/imagemanager.h"

namespace FIFE
{

    class /*FIFE_API*/ GuiImage : public fcn::Image
    {
        public:
            GuiImage();
            explicit GuiImage(ImagePtr const & img);
            ~GuiImage();

            GuiImage(GuiImage const &)            = delete;
            GuiImage& operator=(GuiImage const &) = delete;

            void free();
            int32_t getWidth() const;
            int32_t getHeight() const;
            fcn::Color getPixel(int32_t x, int32_t y);
            void putPixel(int32_t x, int32_t y, fcn::Color const & color);
            void convertToDisplayFormat();
            ImagePtr getFIFEImage() const
            {
                return m_imgPtr;
            }

        protected:
            ImagePtr m_imgPtr;
    };
} // namespace FIFE

#endif
