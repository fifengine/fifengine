/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#ifndef GUI_H
#define GUI_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <sdlfifegraphics.hpp>
#ifdef HAVE_OPENGL
	#include <guichan/opengl.hpp>
	#include <openglfifegraphics.hpp>
#endif
#include "singleton.h"
#include "inputlistener.h"
#include <sdltruetypefont.hpp>
#include "gui_widget.h"

typedef enum {
	BUTTON,
	CHECKBOX,
	LISTBOX,
	RADIOBUTTON,
	SLIDER,
	TEXTBOX,
	BASICCONTAINER,
	CONTAINER,
	DROPDOWN,
	SCROLLAREA,
	WINDOW
} Widget_Type;

namespace FIFE
{

	class GUI : public InputListenerRaw
	{
		public:
			GUI(bool usegl, SDL_Surface *surface);
			~GUI();

			void drawGUI();

			gcn::Container* getContainer();

			void add(gcn::Widget* widget);
			void remove(gcn::Widget* widget);

			std::string registerWidget(const std::string& id, Widget* w);
			void unregisterWidget(const std::string& id);
			Widget* getWidget(const std::string& id);

			void attach(const std::string& id);
			void detach(const std::string& id);
			bool isAttached(const std::string& id);

			virtual void handleEvent(SDL_Event* event);
		private:
			typedef std::map<std::string, Widget*> type_widgets;
			type_widgets m_widgets;

			SDL_Surface* m_surface;

			gcn::SDLInput* m_input;
#ifdef HAVE_OPENGL
			gcn::OpenGLFIFEGraphics* m_glgraphics;
			gcn::OpenGLImageLoader* m_glimageLoader;
#endif
			gcn::SDLFIFEGraphics* m_sdlgraphics;
			gcn::SDLImageLoader* m_sdlimageLoader;

			gcn::Gui* m_gui;
			gcn::Container* m_top;
			gcn::SDLTrueTypeFont* m_font;

			bool m_render_fps_counter;

			GUI& operator=(const GUI&);
	};

}//FIFE

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
