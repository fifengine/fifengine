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
#ifndef FIFE_ENGINE_H
#define FIFE_ENGINE_H

#include <SDL.h>
#include "singleton.h"
#include "inputlistener.h"
#include "console.h"
#include "gui.h"

namespace FIFE {

	class Engine : public Singleton<Engine>, public InputListener {
		public:
			SDL_Surface* createMainScreen(int width, int height, int bpp, bool fullscreen, bool hwsurface, bool use_gl, bool use_db);

			void setGlobalGamma(float);
			float getGlobalGamma() const;

			void start();
			void stop();
			void quit();

			virtual void handleKeyEvent(const SDL_KeyboardEvent& event);

			GUI *getGUI() const;
			int screenWidth() const;
			int screenHeight() const;

		private:
			SDL_Surface* m_screen;
			bool m_run;
			float m_global_gamma;
			GUI *m_gui;

			void initSDL();
			void mainLoop();

			void handleFPSCounter();

			SINGLEFRIEND(Engine);
	};

}//FIFE

#endif
