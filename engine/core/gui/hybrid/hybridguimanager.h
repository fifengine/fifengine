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

#ifndef FIFE_GUI_HYBRIDGUIMANAGER_H
#define FIFE_GUI_HYBRIDGUIMANAGER_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/singleton.h"
#include "gui/guimanager.h"

namespace FIFE {
	
	class HybridGuiManager : 
	public IGUIManager,
	public DynamicSingleton<HybridGuiManager> {
	
	public: 
		
		/** Constructor.
		 */
		HybridGuiManager();
		
		/** Destructor.
		 */
		~HybridGuiManager();
		
		/** Adds a gui manager to this hybrid manager.
		 * 
		 * WARNING This object gains ownership of the added gui manager.
		 */
		void addGuiManager(IGUIManager* guiManager);
		
		/** Removes a gui manager from this hybrid manager.
		 * 
		 * WARNING This object loses owenership of the removed gui manager.
		 */
		void removeGuiManager(IGUIManager* guiManager);
		
		/** Calls turn for each gui manager in the hybrid manager's queue.
		 */
		virtual void turn();
		
		/** Calls onSdlEvent for each gui manager in the hybrid manager's queue.
		 * 
		 * @param event: The event to be processed by each gui manager.
		 * @return A boolean value indicating if the event was consumed by one of the
		 *          gui managers in the queue.
		 */
		virtual bool onSdlEvent(SDL_Event& event);
		
		/** Calls resize top container for each of the gui managers in the queue.
		 */
		virtual void resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		
	private:
		
		/** All gui managers composing this hybrid gui manager.
		 */
		std::vector<IGUIManager*> m_guiManagers;
	};
	
}

#endif // FIFE_GUI_HYBRIDGUIMANAGER_H