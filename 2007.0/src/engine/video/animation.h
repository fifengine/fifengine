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

#ifndef FIFE_ANIMATION_H
#define FIFE_ANIMATION_H

// Standard C++ library includes
#include <cassert>
#include <map>
#include <vector>

// Platform specific includes
#include "util/fifeint.h"

// 3rd party library includes
#include <boost/function.hpp>

// FIFE includes
#include "timeevent.h"

#include "renderable.h"

namespace FIFE {

	class Image;

	/** Animation.
	 *
	 * A container of Images, which are periodically changed.
	 *
	 * @note We maybe need to have 3 different kinds of Animation(s)
	 * One for continuous scenery, one for doors (flipping direction)
	 * and one that is triggered some time but doesn't flip direction.
	 * @warning The Animation takes Ownership of it's contained Images.
	 * @see RenderAble
	 */
	class Animation : public RenderAble {
	private:
		/** Periodically updates all animations.
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
			/** Default constructor.
			 */
			Updater();

			/** Destructor.
			 */
			virtual ~Updater();

			/** Called by TimeManager. Updates all animations.
			 *
			 * @param time The current time in milliseconds.
			 */
			virtual void updateEvent(unsigned long time);
		};
	public:
		/** Constructor.
		 *
		 * Creates a new Animation which can hold numFrames.
		 *
		 * @param numFrames The number of frames this Animation may contain.
		 * @note The first Animation created @b implicitly creates an Updater object.
		 * @b All existing Animations are updated each turn.
		 * @bug There is no way to have the same Animation twice in different states.
		 */ 
		explicit Animation(size_t numFrames);

		/** Destructor.
		 * 
		 * Removes the Animation from the updateList and deletes its contained Images.
		 *
		 * @see updateList(), Animation
		 */
		~Animation();

		/** Sets the frame.
		 *
		 * @param num Frame.
		 * @param image Pointer to Image.
		 */
		void setFrame(size_t num, Image* image);

		/** Sets the frame duration.
		 *
		 * @param lenght Frame duration.
		 */
		void setFrameDuration(uint32_t length);

		/** Gets the frame duration
		 *
		 */
		uint32_t getFrameDuration() const;

		/** Get number of frames
		 *
		 */
		size_t getNumFrames() const;

		/** Sets the current frame.
		 *
		 * @param num Current frame.
		 */
		void setCurrentFrame(size_t num);

		/** Sets the action frame.
		 *
		 * @param num Action frame.
		 */
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
		/** Gets the width of the Animation.
		 *
		 * @return The width of the Animation.
		 */
		virtual unsigned int getWidth() const;
		/** Gets the height of the Animation.
		 *
		 * @return The height of the Animation.
		 */
		virtual unsigned int getHeight() const;
		/** Gets the current frame of the Animation.
		 *
		 * @return The current frame of the Animation.
		 */
		Image* getCurrentFrame();

		/** Sets the X shift of the Animation.
		 *
		 * @param xshift The X shift of the Animation.
		 */
		virtual void setXShift(int xshift);
		/** Sets the Y shift of the Animation.
		 *
		 * @param yshift The Y shift of the Animation.
		 */
		virtual void setYShift(int yshift);
		/** Gets the X shift of the Animation.
		 *
		 * @return The X shift of the Animation.
		 */
		virtual int getXShift() const;
		/** Gets the Y shift of the Animation.
		 *
		 * @return The Y shift of the Animation.
		 */
		virtual int getYShift() const;

		/** Sets a callback when the end of the Animation is reached.
		 *
		 * @param f Function to be executed when the animation finishes.
		 */
		void setOnEndCallback(boost::function0<void> f);
		/** Sets the Image as active or not.
		 *
		 * @param active Is the Animation active?
		 */
		void setIsActive(bool active);
		/** Sets the direction of the Animation.
		 *
		 * @param forward Direction of the Animation.
		 */
		void setDirection(bool forward);

		/** Get one single frame
		 * @param frame The frame to return
		 */
		RenderAble* getFrame(int frame);


	protected:
		// Frames that compose the Animation.
		std::vector<Image*> frames;
/*
		std::vector<int16_t> m_shifts_x;
		std::vector<int16_t> m_shifts_y;
*/

		// X shift of the Animation.
		int16_t m_xshift;
		// Y shift of the Animation.
		int16_t m_yshift;
		// Action frame of the Animation.
		size_t m_action_frame;

		// Current frame of the Animation.
		size_t m_currentFrame;
		// Frame duration.
		uint32_t m_frameDuration;
		uint32_t m_frameChangedAt;
		void flipFrame();

		bool m_canSwitchDirection;
		bool m_animateForward;
		bool m_isActive;

		// Pointers for the previous and next items in the animation linked list.
		Animation* m_next_animation;
		Animation* m_previous_animation;

		// Updater to automate animation updating. Gets created and deleted by
		// the constructor and destructor as necessary.
		// This needs to be static; otherwise only the originally first 
		// Animation could delete the updater.
		static Updater* m_updater;

	private:
		// Pointer to the beginning of the linked list of animations.
		static Animation* m_first_animation;
		// Function to be executed on the end of the Animation.
		boost::function0<void> m_onAnimationEnd;
	};


}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
