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
#include <guichan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/singleton.h"
#include "eventchannel/sdl/ec_isdleventlistener.h"
#include "eventchannel/mouse/ec_imouselistener.h"
#include "eventchannel/key/ec_ikeylistener.h"
#include "eventchannel/widget/ec_iwidgetlistener.h"

namespace gcn {

	class Gui;
	class Container;
	class Widget;
	class SDLInput;
	class Font;
	class FocusHandler;

}


namespace FIFE {

	class ImagePool;
	class GuiImageLoader;
	class Console;
	class IKeyEvent;
	class IMouseEvent;
	class FontBase;

	/* GUI Manager.
	 *
	 * This class controls the GUI system in FIFE.
	 */
	class GUIManager : 
		public DynamicSingleton<GUIManager>, 
		public ISdlEventListener,
		public IKeyListener,
		public IMouseListener,
		public gcn::ActionListener
		 {
		public:
			/** Constructor.
			 */
			GUIManager(IWidgetListener* widgetListener, ImagePool& pool);
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
			 * @param graphics backend specific grapchics object to use
			 * @param screenWidth width for the gui top container
			 * @param screenHeight height for the gui top container
			 */
			void init(gcn::Graphics* graphics, int screenWidth, int screenHeight);
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

			/** Set the global font
			 */
			void setGlobalFont(gcn::Font* font);
			
			/** Gets font with given properties. Note that font will be owned by guimanager
			 */
			FontBase* createFont(const std::string& path, unsigned int size, const std::string& glyphs);

			/** Releases given font.
			 */
			void releaseFont(gcn::Font* font);

			/** Callback from guichan
			 */
			void action(const gcn::ActionEvent & event);

			void onSdlEvent(SDL_Event& evt);
			void keyPressed(IKeyEvent& evt) { evaluateKeyEventConsumption(evt); }
			void keyReleased(IKeyEvent& evt) { evaluateKeyEventConsumption(evt); }
			void mouseEntered(IMouseEvent& evt) { evaluateMouseEventConsumption(evt); }
			void mouseExited(IMouseEvent& evt) { evaluateMouseEventConsumption(evt); }
			void mousePressed(IMouseEvent& evt);
			void mouseReleased(IMouseEvent& evt) { evaluateMouseEventConsumption(evt); }
			void mouseClicked(IMouseEvent& evt) { evaluateMouseEventConsumption(evt); }
			void mouseWheelMovedUp(IMouseEvent& evt) { evaluateMouseEventConsumption(evt); }
			void mouseWheelMovedDown(IMouseEvent& evt) { evaluateMouseEventConsumption(evt); }
			void mouseMoved(IMouseEvent& evt) { evaluateMouseEventConsumption(evt); }
			void mouseDragged(IMouseEvent& evt) { /* do not consume dragging events */ }

		private:
			void evaluateKeyEventConsumption(IKeyEvent& evt);
			void evaluateMouseEventConsumption(IMouseEvent& evt);

			// The Guichan GUI.
			gcn::Gui* m_gcn_gui;
			// Focus handler for input management
			gcn::FocusHandler* m_focushandler;
			// The top container of the GUI.
			gcn::Container* m_gcn_topcontainer;
			// The imageloader.
			GuiImageLoader* m_imgloader;
			// The input controller.
			gcn::SDLInput *m_input;
			// The console.
			Console       *m_console;
			// The fonts used
			std::vector<gcn::Font*> m_fonts;
			// Added widgets
			std::set<gcn::Widget*> m_widgets;

			// instance whom to deliver widget events coming from guichan
			IWidgetListener* m_widgetlistener;
			// pool used for images
			ImagePool& m_pool;
	};

}

#endif
