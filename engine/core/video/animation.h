/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_VIDEO_ANIMATION_H
#define FIFE_VIDEO_ANIMATION_H

// Standard C++ library includes
#include <cassert>
#include <map>
#include <vector>

// Platform specific includes
#include "util/base/fife_stdint.h"
#include "util/resource/resource.h"

#include "image.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	/** Animation.
	 *
	 * A container of Images describing an animation. Animation itself does
	 * not take care of animating the images. Instead it contains images
	 * having associated timestamps. It is the responsibility of the
	 * animation user to query frames based on current timestamp and show
	 * returned images on screen.
	 */
	class Animation : public IResource {
	public:
		/** Constructor.
		*/
		Animation(IResourceLoader* loader = 0);
		Animation(const std::string& name, IResourceLoader* loader = 0);

		/** Destructor. Decreases the reference count of all referred images.
		 */
		~Animation();

		virtual size_t getSize();

		virtual void load();
		virtual void free();

		void invalidate();

		/** Adds new frame into animation
		 * Frames must be added starting from first frame. Increases the reference
		 * count of the given image.
		 * @param image Pointer to Image. Does not transfer the ownership
		 * @param duration Duration for given frame in the animation
		 */
		void addFrame(ImagePtr image, uint32_t duration);

		/** Get the frame index that matches given timestamp. In case there is no exact match,
		 * correct frame is calculated. E.g. if there are frames for timestamps 50 and 100
		 * and frame for 75 is asked, frame associated with value 50 is given back.
		 * In case the timestamp is past the sequence, negative value is returned
		 * @see addFrame
		 */
		int32_t getFrameIndex(uint32_t timestamp);

		/** Gets the frame iamge that matches the given index. If no matches found, returns an invalid ImagePtr
		 */
		ImagePtr getFrame(int32_t index);

		/** Gets the frame image that matches the given timestamp.
		 */
		ImagePtr getFrameByTimestamp(uint32_t timestamp);

		/** Gets all frame images.
		 */
		std::vector<ImagePtr> getFrames();

		/** Gets the frame duration for given (indexed) frame. Returns negative value in case
		 * of incorrect index
		 */
		int32_t getFrameDuration(int32_t index) const;

		/** Get the number of frames
		 */
		uint32_t getFrameCount() const;

		/** Sets the action frame. Action frame is the frame when the related
		 * action actually happens. E.g. in case of punch animation, action frame is
		 * the frame when punch hits the target. In case there is no associated
		 * action frame, value is negative
		 * @param num index of the action frame.
		 */
		void setActionFrame(int32_t num) { m_action_frame = num; }

		/** Gets the action frame.
		 * @see setActionFrame
		 */
		int32_t getActionFrame() const { return m_action_frame; }

		/** Animation direction tells how this animation is associated
		 * with movement when played starting from frame 0
		 * E.g. in walking animation there should be direction assigned so
		 * that engine can choose the correct animation when characters are
		 * moved around the map area
		 * @param direction direction to set
		 */
		void setDirection(uint32_t direction);

		/** Gets the animation direction
		 * @see setDirection
		 * @return direction for this animation
		 */
		uint32_t getDirection() const { return m_direction; }

		/** Gets the total duration for the whole animation
		 */
		uint32_t getDuration() const { return m_animation_endtime; }

	private:
		/** Contains information about one animation frame (duration + frame index + frame pointer)
		 */
		struct FrameInfo {
			uint32_t index;
			uint32_t duration;
			ImagePtr image;
		};
		
		std::string createUniqueAnimationName();

		/** Checks for animation frame index overflows
		 */
		bool isValidIndex(int32_t index) const;

		// Map of timestamp + associated frame
		std::map<uint32_t, FrameInfo> m_framemap;
		// vector of frames for fast indexed access
		std::vector<FrameInfo> m_frames;
		// Action frame of the Animation.
		int32_t m_action_frame;
		// time when animation ends (zero based)
		int32_t m_animation_endtime;
		// Direction for this animation
		uint32_t m_direction;

	};

	typedef SharedPtr<Animation> AnimationPtr;

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
