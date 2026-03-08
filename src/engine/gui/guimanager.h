// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_GUI_GUIMANAGER_H
#define FIFE_VIDEO_GUI_GUIMANAGER_H

// Standard C++ library includes
#include <set>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "eventchannel/sdl/isdleventlistener.h"
#include "util/base/fife_stdint.h"
#include "util/base/singleton.h"

namespace FIFE
{

    class IGUIManager : public ISdlEventListener
    {
    public:
        /** Destructor.
         */
        virtual ~IGUIManager() { };

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
    };
} // namespace FIFE

#endif