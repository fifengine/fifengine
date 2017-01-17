/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes

// 3rd party library includes
#include <cegui-0/CEGUI/CEGUI.h>
#include <cegui-0/CEGUI/RendererModules/OpenGL/GLRenderer.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "base/ceguiinputprocessor.h"
#include "util/time/timemanager.h"

#include "ceguimanager.h"

namespace FIFE {
	
	CEGuiManager::CEGuiManager() {
#ifdef HAVE_OPENGL
		CEGUI::OpenGLRenderer::bootstrapSystem();
#else
		throw GuiException("CEGUI can be used only if opengl is enabled!");
#endif
		m_inputProcessor = new CEGuiInputProcessor();
		
		m_lastTimePulse = TimeManager::instance()->getTime() / 1000.0;

	}
	
	CEGuiManager::~CEGuiManager() {
		delete m_inputProcessor;
		
		CEGUI::OpenGLRenderer::destroySystem();
	}
	
	void CEGuiManager::turn() {
		injectTimePulse();
		
			CEGUI::System::getSingleton().renderAllGUIContexts();
	}
	
	void CEGuiManager::resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	}
	
	bool CEGuiManager::onSdlEvent(SDL_Event &event) {
		return m_inputProcessor->onSdlEvent(event);
	}
	
	void CEGuiManager::setRootWindow(CEGUI::Window* root) {
		m_guiRoot = root;
		CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(m_guiRoot);
	}
	
	CEGUI::Window* CEGuiManager::getRootWindow() {
		return m_guiRoot;
	}
	
	void CEGuiManager::injectTimePulse() {
		
		double timeNow = TimeManager::instance()->getTime() / 1000.0;
		float time_pulse = float(timeNow - m_lastTimePulse);	

		CEGUI::System::getSingleton().injectTimePulse(time_pulse);
		CEGUI::System::getSingleton().getDefaultGUIContext().injectTimePulse(time_pulse);        
		
		m_lastTimePulse = timeNow;
	}
}
