// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

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

namespace FIFE
{
    static Logger _log(LM_GUI);

    GuiImage::GuiImage() = default;

    GuiImage::GuiImage(const ImagePtr& img) : m_imgPtr(img) { }

    GuiImage::~GuiImage() = default;

    void GuiImage::free()
    {
        ImageManager::instance()->free(m_imgPtr->getHandle());
    }

    int32_t GuiImage::getWidth() const
    {
        if (!m_imgPtr) {
            return 0;
        }
        return m_imgPtr->getWidth();
    }

    int32_t GuiImage::getHeight() const
    {
        if (!m_imgPtr) {
            return 0;
        }
        return m_imgPtr->getHeight();
    }

    fcn::Color GuiImage::getPixel([[maybe_unused]] int32_t x, [[maybe_unused]] int32_t y)
    {
        FL_PANIC(_log, "GuiImage::getPixel, not implemented");
        return {};
    }

    void GuiImage::putPixel(
        [[maybe_unused]] int32_t x, [[maybe_unused]] int32_t y, [[maybe_unused]] const fcn::Color& color)
    {
        FL_PANIC(_log, "GuiImage::putPixel, not implemented");
    }

    void GuiImage::convertToDisplayFormat()
    {
        // empty on purpose
        // Since we don't need any conversions - this is just a proxy.
    }
} // namespace FIFE
