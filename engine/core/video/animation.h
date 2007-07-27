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

#ifndef FIFE_VIDEO_ANIMATION_H
#define FIFE_VIDEO_ANIMATION_H

// Standard C++ library includes
#include <cassert>
#include <map>
#include <vector>

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/time/timeevent.h"

#include "renderable.h"

namespace FIFE {

	class Image;
	class Animation;
	class AnimationUpdater;

	/** Listener interface for animations 
	 */
	class AnimationListener {
	public:
		virtual ~AnimationListener() {}
		virtual void onAnimationEnd(Animation* animation) = 0;
	};


	/** Animation.
	 *
	 * A container of Images, which are periodically changed.
	 * @see Renderable
	 */
	class Animation : public Renderable {
	public:
		/** Constructor.
		 */ 
		explicit Animation();

		/** Destructor.
		 */
		~Animation();

		/** Adds new frame into animation
		 * @param image Pointer to Image. Does not transfer ownership
		 */
		void addFrame(Image* image);

		/** Sets the frame duration for all frames.
		 * @param length Frame duration.
		 */
		void setFrameDuration(int length);

		/** Gets the global frame duration for all frames
		 */
		int getFrameDuration() const;

		/** Get number of frames
		 */
		unsigned int getNumFrames() const;

		/** Sets the current frame.
		 * @param num Current frame.
		 */
		void setCurrentFrame(unsigned int num);

		/** Sets the action frame.
		 * @param num Action frame.
		 */
		void setActionFrame(unsigned int num);

		void render(const Rect&, Screen*, unsigned char alpha = 255);

		/** Gets the width of the Animation.
		 * @return The width of the Animation.
		 */
		virtual unsigned int getWidth() const;

		/** Gets the height of the Animation.
		 * @return The height of the Animation.
		 */
		virtual unsigned int getHeight() const;

		/** Sets the X shift of the Animation.
		 * @param xshift The X shift of the Animation.
		 */
		virtual void setXShift(int xshift);

		/** Sets the Y shift of the Animation.
		 * @param yshift The Y shift of the Animation.
		 */
		virtual void setYShift(int yshift);

		/** Gets the X shift of the Animation.
		 * @return The X shift of the Animation.
		 */
		virtual int getXShift() const;

		/** Gets the Y shift of the Animation.
		 * @return The Y shift of the Animation.
		 */
		virtual int getYShift() const;

		/** Sets a callback when the end of the Animation is reached.
		 * @param f Function to be executed when the animation finishes.
		 */
		void setAnimationListener(AnimationListener* listener);

		/** Sets if animation is active or not
		 * @param active Is the Animation active?
		 */
		void setActive(bool active);

		/** Gets if animation is active or not
		 * @return if the Animation is active?
		 */
		bool getActive();

		/** Sets the direction of the Animation.
		 * @param forward Direction of the Animation.
		 */
		void setDirection(bool forward);

		/** Gets animation direction
		 * @return true if running forward
		 */
		bool isDirectionForward();

	private:
		/** Periodically updates the animation.
		*/
		class AnimationUpdater: public TimeEvent {
		public:
			AnimationUpdater(int period, Animation* anim);
	
			virtual ~AnimationUpdater();
	
			virtual void updateEvent(unsigned long time);
	
		private:
			Animation* m_animation;
		};

		/** Sets the next frame in animation
		 */
		void setNextFrame();

		/** Checks for animation frame index overflows
		 */
		void checkIndexOverflow(unsigned int index);

		/** Enables/disables animation's internal timer
		 */
		void setTimerActive(bool active);

		// timer to manage animation updates
		AnimationUpdater* m_updater;
		// Listener for animation
		AnimationListener* m_listener;
		// Frames that compose the Animation.
		std::vector<Image*> m_frames;
		// X shift of the Animation.
		int m_xshift;
		// Y shift of the Animation.
		int m_yshift;
		// Action frame of the Animation.
		unsigned int m_action_frame;
		// Current frame of the Animation.
		unsigned int m_currentFrame;
		// Frame duration.
		int m_frameDuration;
		// defines if animating forward
		bool m_animateForward;
		// defines if animation is active
		bool m_isActive;
	};


}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
