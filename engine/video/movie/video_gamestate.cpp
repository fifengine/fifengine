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
 
// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/gamestate/gamestatemanager.h"
#include "util/time/timemanager.h"
#include "util/log.h"
#include "video/renderbackends/sdl/sdlscreen.h"
#include "video/rendermanager.h"
#include "video/screen.h"

#include "video_gamestate.h"

namespace FIFE {
	VideoGameState::VideoGameState() : GameState("MoviePlayer"), filename("test.avi") {
		sdl_ov = 0;
	}
	
	VideoGameState::~VideoGameState() {}
	
	void VideoGameState::activate() {
		if (filename.length() == 0) {
			Warn(__FILE__) << "No movie set!";
			exit(1);
		}
		RenderManager *rm = RenderManager::instance();
		RenderBackend *actual = rm->current();
		RenderBackend *_sdl = rm->get("SDL");
		
		SDL_Surface * screen = static_cast<SDLScreen*>(_sdl->getMainScreen())->getSurface();
	
		if (_sdl == actual) {
			sdl_ov = new SDL_Video_Overlay(screen);
			try {
				sdl_ov->load(filename);
			}
			catch (const CannotOpenFile & e) {
				Warn(__FILE__) << e.getMessage();
				exit(1);
			}
		} else {
			Warn(__FILE__) << "OpenGL not supported at this time";
			exit(1);
		}
		now_t = TimeManager::instance()->getTime();
		v_fps = 1000 / sdl_ov->getFps();
		Log("video info") << "fps: " << sdl_ov->getFps() << " size: " << 
		sdl_ov->getWidth() << "x" << sdl_ov->getHeight();
		sdl_ov->nextFrame();
	}
	
	void VideoGameState::deactivate() {
		if (sdl_ov)
			delete sdl_ov;
	}
	
	void VideoGameState::turn() {
		unsigned long t = TimeManager::instance()->getTime();
		if (sdl_ov) {
			if (t > now_t + v_fps) {
				if (!sdl_ov->nextFrame()) {
					GameStateManager::instance()->deactivate(getName());
					return;
				}
				now_t = t;
			}
			sdl_ov->showFrame();
		}
	}
	
	void VideoGameState::setFile(const std::string & file) {
		filename = file;
	}
	
	VideoGameState::SDL_Video_Overlay::SDL_Video_Overlay(SDL_Surface * mscreen) : VideoStream() {
		screen = mscreen; 
		rect.x = 0;
		rect.y = 0;
		rect.w = screen->w;
		rect.h = screen->h;
		yuv_overlay = SDL_CreateYUVOverlay(screen->w, screen->h, SDL_IYUV_OVERLAY, screen);
	}
	
	VideoGameState::SDL_Video_Overlay::~SDL_Video_Overlay() {
		SDL_FreeYUVOverlay(yuv_overlay);
	}
	
	void VideoGameState::SDL_Video_Overlay::showFrame() {
		if (SDL_MUSTLOCK(screen))
			SDL_LockSurface(screen);
		SDL_LockYUVOverlay(yuv_overlay);
		
		// set screen to black
		// FIXME: doing this every frame is crazy
		memset(yuv_overlay->pixels[0], 0, yuv_overlay->pitches[0] * yuv_overlay->h);
		memset(yuv_overlay->pixels[1], 128, yuv_overlay->pitches[1] * ((yuv_overlay->h + 1)/2));
		memset(yuv_overlay->pixels[2], 128, yuv_overlay->pitches[2] * ((yuv_overlay->h + 1)/2));
		
		// FIXME: bad assumption: yuv_overlay "larger" than video frame
		int offset_y, offset_u, offset_v,  offset_h;
		// x-offsets in the pixelbuffer; 'center' frame in yuv_overlay horizontal
		offset_y = (yuv_overlay->pitches[0] - frame->linesize[0])/2;
		offset_u = (yuv_overlay->pitches[1] - frame->linesize[1])/2;
		offset_v = (yuv_overlay->pitches[2] - frame->linesize[2])/2;
		
		// y-offset; center vertical
		offset_h = (yuv_overlay->h - codecCtx->height)/2;
		for (int y=0; y<codecCtx->height; y++) {
			memcpy(yuv_overlay->pixels[0]+yuv_overlay->pitches[0]*(y+offset_h) + offset_y, frame->data[0]+frame->linesize[0]*y, codecCtx->width);
		}
		// why /4? seems to work
		offset_h = (yuv_overlay->h - codecCtx->height)/4;
		for (int y=0; y<codecCtx->height/2; y++) {
			memcpy(yuv_overlay->pixels[1]+yuv_overlay->pitches[1]*(y+offset_h) + offset_u, frame->data[1]+frame->linesize[1]*y, codecCtx->width/2);
			memcpy(yuv_overlay->pixels[2]+yuv_overlay->pitches[2]*(y+offset_h) + offset_v, frame->data[2]+frame->linesize[2]*y, codecCtx->width/2);
			// why width/2? seems to work
		}
	
		// this is a algo found in several places (on the web)...
		// but it does freakish things 
		/*
		int offset_y, offset_u, offset_v;
		offset_y = (frame->linesize[0] - yuv_overlay->pitches[0])/2;
		offset_u = (frame->linesize[1] - yuv_overlay->pitches[1])/2;
		offset_v = (frame->linesize[2] - yuv_overlay->pitches[2])/2;
		for (int y=0; y<screen->h; y++) {
		memcpy(yuv_overlay->pixels[0]+yuv_overlay->pitches[0]*y, 
			frame->data[0]+frame->linesize[0]*y+offset_y, yuv_overlay->pitches[0]);	
		}
		for (int y=0; y<screen->h/2; y++) {
		memcpy(yuv_overlay->pixels[1]+yuv_overlay->pitches[1]*y, 
			frame->data[1]+frame->linesize[1]*y+offset_u, yuv_overlay->pitches[1]);	
		memcpy(yuv_overlay->pixels[2]+yuv_overlay->pitches[2]*y, 
			frame->data[2]+frame->linesize[2]*y+offset_v, yuv_overlay->pitches[2]);	
		}
		*/
		
		if (SDL_MUSTLOCK(screen))
			SDL_UnlockSurface(screen);
		SDL_UnlockYUVOverlay(yuv_overlay);
		SDL_DisplayYUVOverlay(yuv_overlay, &rect);
	}
}
