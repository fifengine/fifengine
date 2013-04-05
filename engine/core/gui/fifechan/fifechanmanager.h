/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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

#ifndef FIFE_VIDEO_GUI_GUICHANMANAGER_H
#define FIFE_VIDEO_GUI_GUICHANMANAGER_H

// Standard C++ library includes
#include <set>

// 3rd party library includes
#include <fifechan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"
#include "util/base/singleton.h"
#include "eventchannel/sdl/ec_isdleventlistener.h"

#include "gui/guimanager.h"

namespace fcn {

	class Gui;
	class Container;
	class Widget;
	class SDLInput;
	class FocusHandler;

}


namespace FIFE {

	class GuiImageLoader;
	class Console;
	class KeyEvent;
	class MouseEvent;
	class IFont;
	class GuiFont;


	/* GUI Fifechan Manager.
	 *
	 * This class controls the Fifechan GUI system in FIFE.
	 */
	class FifechanManager :
		public IGUIManager,
		public DynamicSingleton<FifechanManager>
		 {
		public:
			/** Constructor.
			 */
			FifechanManager();
			/** Destructor.
			 */
			virtual ~FifechanManager();

			/** Gets the member pointer to the Fifechan GUI.
			 *
			 * @return The member pointer to the Fifechan GUI.
			 */
			fcn::Gui* getFifechanGUI() const;

			/** Performs the GUI logic and draws the GUI accordingly.
			 *
			 * This will be called each frame.
			 */
			virtual void turn();

			/** Inits the Fifechan GUI Manager.
			 * @param graphics backend specific grapchics object to use
			 * @param screenWidth width for the gui top container
			 * @param screenHeight height for the gui top container
			 */
			void init(const std::string& backend, int32_t screenWidth, int32_t	screenHeight);

			/** Resizes the top container.
			 *
			 * @param x The new starting X coordinate.
			 * @param y The new starting Y coordinate.
			 * @param width The new width.
			 * @param height The new height.
			 */
			void resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

			/** Adds a new widget.
			 *
			 * @param A pointer to the widget to add.
			 */
			void add(fcn::Widget* widget);
			/** Removes a widget.
			 *
			 * @param A pointer to the widget to remove.
			 */
			void remove(fcn::Widget* widget);
			/** Gets the top container.
			 *
			 * @return The top container.
			 */
			fcn::Container* getTopContainer() const { return m_fcn_topcontainer; }

			/** Gets the console.
			 *
			 * @return The console.
			 */
			Console* getConsole() const { return m_console; };

			/** Set the global font properties.
			 */
			GuiFont* setDefaultFont(const std::string& path, uint32_t size, const std::string& glyphs);

			/** Gets the default font
			 */
			GuiFont* getDefaultFont() { return m_defaultfont; };

			/** Gets font with given properties. Note that font will be owned by Fifechan GUI Manager
			 */
			GuiFont* createFont(const std::string& path = "", uint32_t size = 0, const std::string& glyphs = "");

			/** Releases given font.
			 */
			void releaseFont(GuiFont* font);

			void invalidateFonts();

			virtual bool onSdlEvent(SDL_Event& evt);

			KeyEvent translateKeyEvent(const fcn::KeyEvent& evt);
			MouseEvent translateMouseEvent(const fcn::MouseEvent& evt);

		protected:
			static int32_t convertFifechanKeyToFifeKey(int32_t value);

		private:
			// The Fifechan GUI.
			fcn::Gui* m_fcn_gui;
			// Fifechan Graphics
			fcn::Graphics* m_gui_graphics;
			// Focus handler for input management
			fcn::FocusHandler* m_focushandler;
			// The top container of the GUI.
			fcn::Container* m_fcn_topcontainer;
			// The imageloader.
			GuiImageLoader* m_imgloader;
			// The input controller.
			fcn::SDLInput *m_input;
			// The console.
			Console       *m_console;
			//The default font
			GuiFont* m_defaultfont;
			// The fonts used
			std::vector<GuiFont*> m_fonts;
			// Added widgets
			std::set<fcn::Widget*> m_widgets;

			// Used to accept mouse motion events that leave widget space
			bool m_had_mouse;
			bool m_had_widget;

			// default font settings
			std::string m_fontpath;
			std::string m_fontglyphs;
			int32_t m_fontsize;

			// true, if fifechan logic has already been executed for this round
			bool m_logic_executed;
	};

}

#endif
