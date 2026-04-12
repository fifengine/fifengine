// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GUI_IMAGELOADER_H
#define FIFE_GUI_IMAGELOADER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes
#include <fifechan/image.hpp>
#include <fifechan/imageloader.hpp>

// FIFE includes
#include "gui_image.h"

namespace FIFE
{
    class AtlasBook;

    /* Image Loader.
     */
    class /*FIFE_API*/ GuiImageLoader : public fcn::ImageLoader
    {
    public:
        GuiImageLoader();
        virtual ~GuiImageLoader();
        virtual fcn::Image* load(const std::string& filename, bool convertToDisplayFormat = true);

    private:
        AtlasBook* m_atlasbook;
        std::vector<ImagePtr> m_atlases;
    };

} // namespace FIFE

#endif
