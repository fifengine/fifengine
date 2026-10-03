// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_SPEECHSTYLE_H
#define FIFE_SPEECHSTYLE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "video/color.h"

namespace FIFE
{
    class IFont;

    /** Bubble body style.
     *
     *  The names mirror fcn::SpeechBubble::BubbleStyle so both bubble APIs in
     *  FIFE speak the same vocabulary. Only CLASSIC is drawn by
     *  FloatingTextRenderer today; the others fall back to it.
     */
    enum class FIFE_API BubbleType : uint8_t
    {
        NONE = 0,
        CLASSIC,
        ROUND,
        THOUGHT,
        SHOUT,
        WHISPER
    };

    enum class FIFE_API TailDirection : uint8_t
    {
        DOWN = 0,
        UP,
        LEFT,
        RIGHT,
        AUTO
    };

    enum class FIFE_API TextAlign : uint8_t
    {
        LEFT = 0,
        CENTER,
        RIGHT
    };

    struct FIFE_API SpeechStyle
    {
            BubbleType bubbleType     = BubbleType::NONE;
            int bubblePadding         = 5;
            int cornerRadius          = 8;
            int tailSize              = 10;
            int tailOffset            = 0;
            TailDirection tailDir     = TailDirection::DOWN;
            int maxTextWidth          = 0;
            TextAlign textAlign       = TextAlign::LEFT;
            IFont* fontOverride       = nullptr;
            bool hasTextColorOverride = false;
            Color textColorOverride{};
            bool hasBackgroundOverride = false;
            Color backgroundColorOverride{};
            bool hasBorderOverride = false;
            Color borderColorOverride{};

            static SpeechStyle plain()
            {
                return SpeechStyle();
            }

            static SpeechStyle classic()
            {
                SpeechStyle s;
                s.bubbleType = BubbleType::CLASSIC;
                return s;
            }
    };
} // namespace FIFE

#endif
