/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef FIFE_VIDEO_GUI_GUIMANAGER_H
#define FIFE_VIDEO_GUI_GUIMANAGER_H

// Standard C++ library includes
#include <set>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/singleton.h"

namespace gcn {

	class Gui;
	class Container;
	class Widget;
	class SDLInput;
	class Font;

}


namespace FIFE {

	class GCNImageLoader;
	class Console;

	/* GUI Manager.
	 *
	 * This class controls the GUI system in FIFE.
	 */
	class GUIManager : public DynamicSingleton<GUIManager> {
		public:
			/** Constructor.
			 */
			GUIManager();
			/** Destructor.
			 */
			virtual ~GUIManager();

			/** Gets the member pointer to the Guichan GUI.
			 *
			 * @return The member pointer to the Guichan GUI.
			 */
			gcn::Gui* getGuichanGUI() const;

			/** Performs the GUI logic and draws the GUI accordingly.
			 *
			 * This will be called each frame.
			 */
			void turn();
			/** Inits the GUI Manager.
			 */
			void init();
			/** Resizes the top container.
			 *
			 * @param x The new starting X coordinate.
			 * @param y The new starting Y coordinate.
			 * @param width The new width.
			 * @param height The new height.
			 */
			void resizeTopContainer(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
			/** Adds a new widget.
			 *
			 * @param A pointer to the widget to add.
			 */
			void add(gcn::Widget* widget);
			/** Removes a widget.
			 *
			 * @param A pointer to the widget to remove.
			 */
			void remove(gcn::Widget* widget);
			/** Gets the top container.
			 *
			 * @return The top container.
			 */
			gcn::Container* getTopContainer() { return m_gcn_topcontainer; }
			/** Gets the console.
			 *
			 * @return The console.
			 */
			Console* getConsole() { return m_console; };

			/** Set the default global font
			 * \param font A gcn::Font or NULL
			 * If NULL is given as parameter, the default font from the
			 * config file is taken.
			 */
			void setGlobalFont(gcn::Font* font);

			/** Gets default fotn
			 *
			 * @return Default font.
			 */
			gcn::Font* getDefaultFont() { return m_font; };

		private:
			// The Guichan GUI.
			gcn::Gui* m_gcn_gui;
			// The top container of the GUI.
			gcn::Container* m_gcn_topcontainer;
			// The imageloader.
			GCNImageLoader* m_gcn_imgloader;
			// The input controller.
			gcn::SDLInput *m_input;
			// The console.
			Console       *m_console;
			// The standard font
			gcn::Font     *m_font;
			// Added widgets
			std::set<gcn::Widget*> m_widgets;
	};

}

#endif
