// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes

// 3rd party library includes
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GLRenderer.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "base/ceguiinputprocessor.h"
#include "util/time/timemanager.h"

#include "ceguimanager.h"

namespace FIFE
{

    CEGuiManager::CEGuiManager()
    {
#ifdef HAVE_OPENGL
        CEGUI::OpenGLRenderer::bootstrapSystem();
        dynamic_cast<CEGUI::OpenGLRenderer*>(CEGUI::System::getSingleton().getRenderer())
            ->enableExtraStateSettings(true);
#else
        throw GuiException("CEGUI can be used only if opengl is enabled!");
#endif
        m_inputProcessor = new CEGuiInputProcessor();

        m_lastTimePulse = TimeManager::instance()->getTime() / 1000.0;
    }

    CEGuiManager::~CEGuiManager()
    {
        delete m_inputProcessor;

        CEGUI::OpenGLRenderer::destroySystem();
    }

    void CEGuiManager::turn()
    {
        injectTimePulse();

        CEGUI::System::getSingleton().renderAllGUIContexts();
    }

    void CEGuiManager::resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        CEGUI::System::getSingleton().notifyDisplaySizeChanged(CEGUI::Sizef(width, height));
    }

    bool CEGuiManager::onSdlEvent(SDL_Event& event)
    {
        return m_inputProcessor->onSdlEvent(event);
    }

    void CEGuiManager::setRootWindow(CEGUI::Window* root)
    {
        m_guiRoot = root;
        CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(m_guiRoot);
    }

    CEGUI::Window* CEGuiManager::getRootWindow()
    {
        return m_guiRoot;
    }

    void CEGuiManager::injectTimePulse()
    {

        double timeNow   = TimeManager::instance()->getTime() / 1000.0;
        float time_pulse = float(timeNow - m_lastTimePulse);

        CEGUI::System::getSingleton().injectTimePulse(time_pulse);
        CEGUI::System::getSingleton().getDefaultGUIContext().injectTimePulse(time_pulse);

        m_lastTimePulse = timeNow;
    }
} // namespace FIFE