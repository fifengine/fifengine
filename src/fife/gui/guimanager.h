// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_GUI_GUIMANAGER_H
#define FIFE_VIDEO_GUI_GUIMANAGER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <set>

// 3rd party library includes

// FIFE includes
#include "eventchannel/sdl/isdleventlistener.h"
#include "util/base/fife_stdint.h"
#include "util/base/singleton.h"

namespace FIFE
{

    class FIFE_API IGUIManager : public ISdlEventListener
    {
        public:
            /** Destructor.
             */
            ~IGUIManager() override = default;

            /** Performs the GUI logic and draws the GUI accordingly.
             *
             * This will be called each frame.
             */
            virtual void turn() = 0;

            /** Resizes the top container.
             *
             * @param x The new starting X coordinate.
             * @param y The new starting Y coordinate.
             * @param width The new width.
             * @param height The new height.
             */
            virtual void resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

            /** Applies the GUI scale factor used for fonts and metrics.
             *
             *  This is not the window pixel/point ratio: it is the extra scale
             *  needed on top of what the renderer already applies (see
             *  calculateGUIScale()). The default implementation ignores the
             *  value so existing GUI managers keep working.
             *
             *  @param scale GUI scale factor (1.0 = 100%).
             */
            virtual void setDPIScale([[maybe_unused]] float scale)
            {
            }

            /** @return The GUI scale factor last passed to setDPIScale(). */
            virtual float getDPIScale() const
            {
                return 1.0F;
            }
    };
} // namespace FIFE

#endif
