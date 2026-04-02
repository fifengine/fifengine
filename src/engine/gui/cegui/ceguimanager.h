// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GUI_CEGuiManager
#define FIFE_GUI_CEGuiManager

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/singleton.h"

#include "gui/guimanager.h"

namespace CEGUI
{
    class Window;
}

namespace FIFE
{

    class CEGuiInputProcessor;

    class CEGuiManager : public IGUIManager, public DynamicSingleton<CEGuiManager>
    {

    public:
        /** Constructor.
         */
        CEGuiManager();

        virtual ~CEGuiManager();

        /** Updates and renders the gui.
         */
        virtual void turn();

        /** Resizes the top container.
         *
         * @param x The new starting X coordinate.
         * @param y The new starting Y coordinate.
         * @param width The new width.
         * @param height The new height.
         */
        virtual void resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        /**
         * TODO remove?
         * Receives input and converts it to librocket format, then it forwards it
         * to librocket.
         */
        virtual bool onSdlEvent(SDL_Event& event);

        /** Sets the root window of the gui system.
         */
        void setRootWindow(CEGUI::Window* root);

        /** @return The root window of the gui system.
         */
        CEGUI::Window* getRootWindow();

    private:
        /** Inject a time pulse to CEGUI.
         */
        void injectTimePulse();

        /** Input processor. Converts SDL input to CEGUI input.
         */
        CEGuiInputProcessor* m_inputProcessor;

        /** Last time pulse injected to CEGUI.
         */
        double m_lastTimePulse;

        /** Our root gui window.
         */
        CEGUI::Window* m_guiRoot;
    };
} // namespace FIFE

#endif // FIFE_GUI_CEGuiManager
