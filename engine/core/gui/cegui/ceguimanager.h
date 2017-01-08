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

namespace CEGUI {
	class Window;
}

namespace FIFE {
	
	class CEGuiInputProcessor;
	
	class CEGuiManager : 
	public IGUIManager,
	public DynamicSingleton<CEGuiManager> {
	
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
		
		/** Receives input and converts it to librocket format, then it forwards it
		 * to librocket.
		 */
		virtual bool onSdlEvent(SDL_Event& event);
		
		/** Sets the root window of the gui system.
		 */
		void setRootWindow(CEGUI::Window* setRootWindow);
		
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
}

#endif //FIFE_GUI_CEGuiManager
