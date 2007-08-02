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
#include "util/resource/pooled_resource.h"

namespace FIFE {

	class Image;

	/** Animation.
	 *
	 * A container of Images describing an animation. Animation itself does 
	 * not take care of animating the images. Instead it contains images
	 * having associated timestamps. It is the responsibility of the 
	 * animation user to query frames based on current timestamp and show
	 * returned images on screen.
	 */
	class Animation: public IPooledResource {
	public:
		/** Constructor.
		 */ 
		explicit Animation();

		/** Destructor. Decreases the reference count of all referred images.
		 */
		~Animation();

		/** Adds new frame into animation
		 * Frames must be added starting from first frame. Increases the reference
		 * count of the given image.
		 * @param image Pointer to Image. Does not transfer the ownership
		 * @param duration Duration for given frame in the animation
		 */
		void addFrame(Image* image, unsigned int duration);

		/** Get the frame index that matches given timestamp. In case there is no exact match,
		 * correct frame is calculated. E.g. if there are frames for timestamps 50 and 100
		 * and frame for 75 is asked, frame associated with value 50 is given back.
		 * In case the timestamp is past the sequence, negative value is returned
		 * @see addFrame
		 */
		int getFrameIndex(unsigned int timestamp);

		/** Gets the frame that matches the given index. If no matches found, returns NULL
		 */
		Image* getFrame(int index);

		/** Gets the frame duration for given (indexed) frame. Returns negative value in case
		 * of incorrect index
		 */
		int getFrameDuration(int index);

		/** Get the number of frames
		 */
		unsigned int getNumFrames() const;

		/** Sets the action frame. Action frame is the frame when the related
		 * action actually happens. E.g. in case of punch animation, action frame is
		 * the frame when punch hits the target. In case there is no associated
		 * action frame, value is negative
		 * @param num index of the action frame.
		 */
		void setActionFrame(int num) { m_action_frame = num; }

		/** Gets the action frame.
		 * @see setActionFrame
		 */
		int getActionFrame() { return m_action_frame; }

		void addRef() { m_refcount++; };
		void decRef() { m_refcount--; };
		unsigned int getRefCount() { return m_refcount; };


	private:
		/** Contains information about one animation frame (duration + frame index + frame pointer)
		 */
		struct FrameInfo {
			unsigned int index;
			unsigned int duration;
			Image* img;
		};

		/** Checks for animation frame index overflows
		 */
		bool isValidIndex(int index);

		// Map of timestamp + associated frame
		std::map<unsigned int, FrameInfo> m_framemap;
		// vector of frames for fast indexed access
		std::vector<FrameInfo> m_frames;
		// Action frame of the Animation.
		int m_action_frame;
		// time when animation ends (zero based)
		int m_animation_endtime;
		// Reference count of this animation
		unsigned int m_refcount;

	};


}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
