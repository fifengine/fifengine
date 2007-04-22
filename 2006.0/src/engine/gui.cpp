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
#include "gui.h"
#include "log.h"
#include "settingsmanager.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <sdlfifegraphics.hpp>
#ifdef HAVE_OPENGL
#include <openglfifegraphics.hpp>
#endif

#include <algorithm>
#include "graphix_primitives.h"
#include "inputmanager.h"
#include "debugutils.h"

namespace FIFE
{

	GUI::GUI(bool usegl, SDL_Surface *surface) : m_font(0)
	{
		try
		{
			m_surface = surface;
			m_render_fps_counter = true;

#ifdef HAVE_OPENGL
			m_glgraphics = NULL;
			m_sdlgraphics = NULL;
			m_glimageLoader = NULL;
#endif

			m_sdlimageLoader = new gcn::SDLImageLoader();
#ifdef HAVE_OPENGL
			if(usegl) {
				m_glimageLoader = new gcn::OpenGLImageLoader(m_sdlimageLoader);
				gcn::Image::setImageLoader(m_glimageLoader); 
			} else
#endif
				gcn::Image::setImageLoader(m_sdlimageLoader); 

#ifdef HAVE_OPENGL
			if(usegl) {
				m_glgraphics = new gcn::OpenGLFIFEGraphics();
				m_glgraphics->setTargetPlane(m_surface->w, m_surface->h);
				m_glgraphics->setColor(gcn::Color(255,255,255,255));
			} else {
#endif
				m_sdlgraphics = new gcn::SDLFIFEGraphics();
				m_sdlgraphics->setTarget(m_surface);
				m_sdlgraphics->setColor(gcn::Color(255,255,255,255));
#ifdef HAVE_OPENGL
			}
#endif
			m_input = new gcn::SDLInput();

			m_top = new gcn::Container();
			m_top->setOpaque(false);

			m_top->setDimension(gcn::Rectangle(0, 0, m_surface->w, m_surface->h));
			m_gui = new gcn::Gui();
#ifdef HAVE_OPENGL
			if(usegl)
				m_gui->setGraphics(m_glgraphics);
			else
#endif
				m_gui->setGraphics(m_sdlgraphics);

			m_gui->setInput(m_input);
			m_gui->setTop(m_top);
			SettingsManager* settings = SettingsManager::instance();
			std::string guifont = settings->read("GUIFont", std::string("lib/fonts/FreeMono.ttf"));
			int guifontsize = settings->read("GUIFontSize", 16);
			m_font = new gcn::SDLTrueTypeFont(guifont, guifontsize);
			m_font->setAntiAlias(true);
			gcn::Widget::setGlobalFont(m_font);

		}
		catch (gcn::Exception e)
		{
			Warn("GUI:") << e.getMessage();
		}
		catch (std::exception e)
		{
			Warn("GUI:") << "Std exception: " << e.what();  
		}
		catch (...)
		{
			Warn("GUI: Unknown exception");
		}

		InputManager::instance()->setForcedListener( this );
	}

	GUI::~GUI()
	{
		InputManager::instance()->setForcedListener( 0 );

		type_widgets::iterator i = m_widgets.begin();
		while (i != m_widgets.end()) {
			printf("deleting %s\n", i->first.c_str());
			delete i->second;
			i++;
		}
		m_widgets.clear();
		if (m_font) delete m_font;
		if (m_top) delete m_top;
		if (m_gui) delete m_gui;

		if (m_input) delete m_input;
		if (m_sdlgraphics) delete m_sdlgraphics;
		if (m_sdlimageLoader) delete m_sdlimageLoader;
#ifdef HAVE_OPENGL
		if (m_glgraphics) delete m_glgraphics;
		if (m_glimageLoader) delete m_glimageLoader;
#endif
	}

	GUI& GUI::operator=(const GUI&)
	{
		return *this;
	}

	void GUI::handleEvent(SDL_Event* event)
	{
		/* don't sent keyup to the gui! strange, but otherwise key events appear twice */
		if (event->type == SDL_KEYUP)
			return;
		m_input->pushInput(*event);
	}

	void GUI::add(gcn::Widget* widget)
	{
		m_top->add(widget);
	}
	void GUI::remove(gcn::Widget* widget)
	{
		m_top->remove(widget);
	}

	void GUI::drawGUI()
	{
		if (!m_surface) return;

		try
		{
			m_gui->logic();
			m_gui->draw();
		}
		catch (gcn::Exception e)
		{
			Warn("GUI:") << e.getMessage();
		}
		catch (std::exception e)
		{
			Warn("GUI:") << "Std exception: " << e.what();  
		}
		catch (...)
		{
			Warn("GUI: Unknown exception");
		}
	}

	std::string GUI::registerWidget(const std::string& id, Widget* w)
	{
		// FIXME: handle name clashes plus unnamed widgets.
		m_widgets[ id ] = w;
		return id;
	};

	void GUI::unregisterWidget(const std::string& id)
	{
		m_widgets.erase( id );
	};

	Widget* GUI::getWidget(const std::string& id)
	{
		return m_widgets[ id ];
	};

	void GUI::attach(const std::string& id)
	{
		Widget* w = getWidget( id );
		if( w == 0 ){
			Warn("gui") << " Can't attach widget " << id << " - it's not registered!";
		} else {
			w->attach();
		};
	};

	void GUI::detach(const std::string& id)
	{
		Widget* w = getWidget( id );
		if( w == 0 ){
			Warn("gui") << " Can't detach widget " << id << " - it's not registered!";
		} else {
			w->detach();
		};
	};

	bool GUI::isAttached(const std::string& id)
	{
		Widget* w = getWidget( id );
		if( w == 0 ){
			Warn("gui")
				<< " Can't ask widget " << id 
				<< " - it's not registered! Returning false from GUI::isAttached().";
			return false;
		} else {
			return w->isAttached();
		};
	};

}//FIFE
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
