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
#ifndef FIFEANIMATION
#define FIFEANIMATION
#include <SDL.h>
#include <vector>
#include <map>
#include <stdint.h>
#include <assert.h>

namespace FIFE {

	class Animation {
		public:
			Animation(size_t numFrames);
			~Animation();

			void setFrame(size_t num, size_t cacheId, int16_t x_shift, int16_t y_shift);
			void setFrameDuration(Uint32 length);
			void setCurrentFrame(size_t num);

			void update( Uint32 nowTicks);
			/*
				 void render(SDL_Rect *target, SDL_Surface *screen);
				 void render(SDL_Rect *target, SDL_Surface *screen, Uint8 alpha);
				 */
			size_t getCurrentFrame();
			int16_t getCurrentShiftX();
			int16_t getCurrentShiftY();
		protected:
			std::vector<size_t> frames;
			std::vector<int16_t> m_shifts_x;
			std::vector<int16_t> m_shifts_y;
			size_t currentFrame;
			Uint32 frameDuration;
			Uint32 frameChangedAt;
			void flipFrame();
			bool animateForward;
	};

	class ComplexAnimation {
		public:
			ComplexAnimation();
			~ComplexAnimation();

			void registerAnimation(uint8_t action, uint8_t direction, Animation *anim);
			void setup(uint8_t startAction, uint8_t startDirection);

			Animation *activeAnimation;
			void setDirection(uint8_t directionId);
			void setAction(uint8_t actionId);

		protected:
			typedef std::map<uint8_t, Animation*> T_animByDirection;
			typedef std::map<uint8_t, T_animByDirection > T_animByAction;
			T_animByAction content;
			//std::map<uint8_t action, std::map<uint8_t direction, Animation *anim > > content;
			uint8_t nowAction;
			uint8_t nowDirection;
		private:
			void cleanup();
	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
