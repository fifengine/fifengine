// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FIFECHAN_ADDON_COMMANDLINE_H
#define FIFE_FIFECHAN_ADDON_COMMANDLINE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <functional>
#include <string>
#include <vector>

// 3rd party library includes
#include <fifechan.hpp>

// FIFE includes
#include "util/time/timer.h"

namespace FIFE
{

    /** A Command line widget
     */
    class FIFE_API CommandLine : public fcn::TextField
    {
        public:
            using type_callback = std::function<void(std::string)>;
            /** Constructor
             */
            CommandLine();

            /** Destructor
             */
            ~CommandLine();

            CommandLine(CommandLine const &)            = delete;
            CommandLine& operator=(CommandLine const &) = delete;

            void keyPressed(fcn::KeyEvent& keyEvent);
            virtual void drawCaret(fcn::Graphics* graphics, int32_t x);

            /** Set callback on pressing the ENTER key
             */
            void setCallback(type_callback const & cb);

            /** Toggle the caret visibility
             */
            void toggleCaretVisible();

            /** Start blinking the caret
             */
            void startBlinking();

            /** Stop blinking the caret for a few seconds
             */
            void stopBlinking();

        private:
            type_callback m_callback;
            std::vector<std::string> m_history;
            size_t m_history_position;
            std::string m_cmdline;
            bool m_caretVisible;
            Timer m_blinkTimer;
            Timer m_suppressBlinkTimer;
    };
} // namespace FIFE

#endif
