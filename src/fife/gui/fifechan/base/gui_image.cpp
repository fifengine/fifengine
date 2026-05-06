// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "gui_image.h"

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "util/log/logger.h"
#include "video/image.h"
#include "video/imagemanager.h"

namespace FIFE
{
    static Logger _log(LM_GUI);

    GuiImage::GuiImage() = default;

    GuiImage::GuiImage(ImagePtr const & img) : m_imgPtr(img)
    {
    }

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
        [[maybe_unused]] int32_t x, [[maybe_unused]] int32_t y, [[maybe_unused]] fcn::Color const & color)
    {
        FL_PANIC(_log, "GuiImage::putPixel, not implemented");
    }

    void GuiImage::convertToDisplayFormat()
    {
        // empty on purpose
        // Since we don't need any conversions - this is just a proxy.
    }
} // namespace FIFE
