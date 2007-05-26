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
 
#ifndef FIFE_VIDEO_MOVIE_VIDEO_GAMESTATE_H
#define FIFE_VIDEO_MOVIE_VIDEO_GAMESTATE_H

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <SDL_video.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/gamestate/gamestate.h"

#include "videostream.h"

namespace FIFE {
	/** Gamestate to display a video.
	 *
	 * \Bug only SDL supported
	 * \Bug bad frame-blitting function
	 */
	class VideoGameState : public GameState {
		public:
			VideoGameState(IEventController& ec);
			~VideoGameState();
			void activate();
			void deactivate();
			void turn();
			void setFile(const std::string & file);
		private:
			/** Helper to blit a video-stream frame to a YUV overlay.
			*
			* \Bug assumes the overlay is larger than the video.
			*/
			struct SDL_Video_Overlay : public VideoStream {
				SDL_Video_Overlay(SDL_Surface*);
				~SDL_Video_Overlay();
				SDL_Overlay *yuv_overlay;
				SDL_Surface *screen;
				SDL_Rect     rect;
				void showFrame();
			};
			unsigned long now_t;
			float v_fps;
			
			SDL_Video_Overlay * sdl_ov;
			std::string filename;
	};
}

#endif
