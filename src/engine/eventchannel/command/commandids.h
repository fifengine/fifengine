// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_ICOMMANDIDS_H
#define FIFE_EVENTCHANNEL_ICOMMANDIDS_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
//

// 3rd party library includes
//
#include <SDL.h>

// FIFE includes
//

namespace FIFE
{
    /**  Types for different commands
     */
    enum CommandType : int8_t
    {
        CMD_UNKNOWN            = -1,
        CMD_QUIT_GAME          = SDL_WINDOWEVENT_CLOSE,
        CMD_MOUSE_FOCUS_GAINED = SDL_WINDOWEVENT_ENTER,
        CMD_MOUSE_FOCUS_LOST   = SDL_WINDOWEVENT_LEAVE,
        CMD_INPUT_FOCUS_GAINED = SDL_WINDOWEVENT_FOCUS_GAINED,
        CMD_INPUT_FOCUS_LOST   = SDL_WINDOWEVENT_FOCUS_LOST,
        CMD_APP_RESTORED       = SDL_WINDOWEVENT_SHOWN,
        CMD_APP_ICONIFIED      = SDL_WINDOWEVENT_HIDDEN,
    };

} // namespace FIFE

#endif