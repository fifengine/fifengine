/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#ifndef FIFEANIMATION
#define FIFEANIMATION

#include <vector>
#include <map>
#include <stdint.h>
#include <assert.h>
#include <boost/function.hpp>

#include "renderable.h"
#include "../timeevent.h"

namespace FIFE {

	class Image;
	/** Animation
	 *
	 * A container of Images, which are periodically changed.
	 *
	 * @note We maybe need to have 3 different kinds of Animation(s)
	 * One for continues scenery, one for doors ( flipping direction )
	 * and one that is triggered some time but doesn't flip direction.
	 *
	 * @warning The Animation takes Ownership of it's contained Images
	 *
	 * @see RenderAble
	 */
	class Animation : public RenderAble {
	private:
		/**  Periodically updates all animations.
		 *
		 * Every frame this object gets updated by TimeManager.
		 * It then calls the first updateList function for the
		 * first Animation in the linked list which then calls
		 * the rest.
		 *
		 * @see TimeEvent, TimeManager, Animation
		 */
		class Updater: public TimeEvent { // Why is this public?
		public:
			/** Default constructor
			 */
			Updater();

			/** Destructor
			 */
			virtual ~Updater();

			/** Called by TimeManager. Updates all animations.
			 *
			 * @param time The current time in milliseconds.
			 */
			virtual void updateEvent(unsigned long time);
		};
	public:
		/** Constructor
		 *
		 * Creates a new Animation which can hold numFrames.
		 * @param numFrames The number of frames this Animation may contain.
		 * @note The first Animation created @b implicitly creates an Updater object.
		 * @b All existing Animations are updated each turn.
		 * @bug There is no way to have the same Animation twice in different states.
		 */ 
		explicit Animation(size_t numFrames);

		/** Destructor
		 * 
		 * Removes the Animation from the updateList and deletes its contained Images
		 *
		 * @see updateList, Animation
		 */
		~Animation();

		void setFrame(size_t num, Image* image);
		void setFrameDuration(uint32_t length);
		void setCurrentFrame(size_t num);
		void setActionFrame(size_t num);

		void update( uint32_t nowTicks);
		/*
				void render(SDL_Rect *target, SDL_Surface *screen);
				void render(SDL_Rect *target, SDL_Surface *screen, Uint8 alpha);
				*/

		/** Called by Updater.
		 *
		 * Calls the current objects update() function and then
		 * calls the updateList() function for the next animation
		 * in the list.
		 *
		 * @param time The current time in milliseconds.
		 */
		void updateList(unsigned long time);

		void render(const Rect&, Screen*, unsigned char alpha = 255);
		virtual unsigned int getWidth() const;
		virtual unsigned int getHeight() const;
		Image* getCurrentFrame();

		virtual void setXShift(int xshift);
		virtual void setYShift(int yshift);
		virtual int getXShift() const;
		virtual int getYShift() const;

		void setOnEndCallback(boost::function0<void> f);
		void setIsActive(bool active);
		void setDirection(bool forward);

	protected:
		std::vector<Image*> frames;
/*
		std::vector<int16_t> m_shifts_x;
		std::vector<int16_t> m_shifts_y;
*/
		int16_t m_xshift;
		int16_t m_yshift;
		size_t m_action_frame;

		size_t m_currentFrame;
		uint32_t m_frameDuration;
		uint32_t m_frameChangedAt;
		void flipFrame();

		bool m_canSwitchDirection;
		bool m_animateForward;
		bool m_isActive;

		// pointers for the previous and next items in the animation linked list.
		Animation* m_next_animation;
		Animation* m_previous_animation;

		// updater to automate animation updating. Gets created and deleted by
		// the constructor and destructor as nessecary.
		// This needs to be static; otherwise only the originally first 
		// Animation could delete the updater.
		static Updater* m_updater;

	private:
		// pointer to the beginning of the linked list of animations.
		static Animation* m_first_animation;
		boost::function0<void> m_onAnimationEnd;
	};


}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
