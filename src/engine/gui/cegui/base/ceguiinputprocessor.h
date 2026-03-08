// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GUI_CEGuiInputProcessor
#define FIFE_GUI_CEGuiInputProcessor

// Standard C++ library includes

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE
{

    class CEGuiInputProcessor
    {
    public:
        /** Constructor.
         */
        CEGuiInputProcessor();

        /** Destructor.
         */
        ~CEGuiInputProcessor();

        /** Injects input to the CEGUI system.
         *
         * @return A boolean value indicating if the event was consumed or not.
         */
        bool onSdlEvent(SDL_Event& event);

    private:
        /** Process a key input event.
         */
        bool processKeyInput(SDL_Event& event);

        /** Process a text input event.
         */
        bool processTextInput(SDL_Event& event);

        /** Process a mouse input event.
         */
        bool processMouseInput(SDL_Event& event);

        /** Process a mouse motion event.
         */
        bool processMouseMotion(SDL_Event& event);

        /** Initialize the key translation map.
         */
        void initializeKeyMap();

        /** Holds translation of key scancodes from SDL to CEGUI.
         */
        std::map<SDL_Keycode, CEGUI::Key::Scan> m_keymap;
    };

} // namespace FIFE

#endif // FIFE_GUI_CEGuiInputProcessor