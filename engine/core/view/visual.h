/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_VIEW_VISUAL_H
#define FIFE_VIEW_VISUAL_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/abstractvisual.h"
#include "util/math/angles.h"
#include "util/structures/rect.h"

namespace FIFE {
	class Object;
	class Instance;
	class Action;
	class Image;
	class Camera;
	
	/** Base class for all 2 dimensional visual classes
	 * Visual classes are extensions to visualize the stuff in model (e.g. instances)
	 * The reason why its separated is to keep model view-agnostic, so that we could
	 * have e.g. 3d, 2d and character based visualizations to the same data
	 */
	class Visual2DGfx: public AbstractVisual {
	public:
		/** Destructor
		 */
		virtual ~Visual2DGfx();

		/** Sets transparency value for object to be visualized
		 *  @param stackposition new stack position
		 */
		void setTransparency(uint8_t transparency) { m_transparency = transparency; }

		/** Gets current transparency value (0-255)
		 *  @return current transparency value
		 */
		unsigned int getTransparency() { return m_transparency; }

		/** Sets visibility value for object to be visualized
		 *  @param visible is object visible or not
		 */
		void setVisible(bool visible) { m_visible = visible; }

		/** Is instance visible or not
		 *  @return is instance visible or not
		 */
		unsigned int isVisible() { return m_visible; }

	protected:
		/** Constructor
		 */
		Visual2DGfx();

		uint8_t m_transparency;
		uint8_t m_visible;

	};

	/** Object visual contains data that is needed for visualizing objects
	 */
	class ObjectVisual: public Visual2DGfx {
	public:
		/** Constructs and assigns it to the passed item
		 */
		static ObjectVisual* create(Object* object);

		/** Destructor
		 */
		virtual ~ObjectVisual();

		/** Adds new static image with given angle (degrees)
		 * Static images are used in case there are no actions active in the instance
		 * There can be several static images for different angles, that are used in
		 * case view / layer is rotated
		 * In case there are no exact matches for current view angles, closest one is used
		 * @param angle angle for image. 0 degrees starts from right and turns counter-clockwise
		 *              (normal math notation)
		  @param image_index index of image to use for given degress
		 */
		void addStaticImage(unsigned int angle, int image_index);

		/** Returns closest matching static image for given angle
		 * @return id for static image
		 */
		int getStaticImageIndexByAngle(int angle);

		/** Returns closest matching image angle for given angle
		 * @return closest matching angle
		 */
		int getClosestMatchingAngle(int angle);

		/** Returns list of available static image angles for this object
		 */
		void getStaticImageAngles(std::vector<int>& angles);

	private:
		/** Constructor
		 */
		ObjectVisual();

		type_angle2id m_angle2img;
	};


	/** Instance visual contains data that is needed to visualize the instance on screen
	 */
	class InstanceVisual: public Visual2DGfx {
	public:
		/** Constructs and assigns it to the passed item
		 */
		static InstanceVisual* create(Instance* instance);

		/** Destructor
		 */
		virtual ~InstanceVisual();

		/** Sets stack position of the instance
		 *  Stack position is used to define the order in which instances residing
		 *  in the same location are drawn
		 *  @param stackposition new stack position
		 */
		void setStackPosition(int stackposition) { m_stackposition = stackposition; }

		/** Gets current stack position of instance
		 *  @return current stack position
		 */
		int getStackPosition() { return m_stackposition; }

	private:
		/** Constructor
		 */
		InstanceVisual();
		int m_stackposition;
	};

	/** Action visual contains data that is needed to visualize different actions on screen
	 */
	class ActionVisual: public Visual2DGfx {
	public:
		/** Constructs and assigns it to the passed item
		 */
		static ActionVisual* create(Action* action);

		/** Destructor
		 */
		virtual ~ActionVisual();

		/** Adds new animation with given angle (degrees)
		 */
		void addAnimation(unsigned int angle, int animation_index);

		/** Gets index to animation closest to given angle
		 * @return animation index, -1 if no animations available
		 */
		int getAnimationIndexByAngle(int angle);

		/** Returns list of available angles for this Action
		 */
		void getActionImageAngles(std::vector<int>& angles);

	private:
		/** Constructor
		 */
		ActionVisual();

		// animations associated with this action (handles to pool)
		//   mapping = direction -> animation
		type_angle2id m_animations;
	};

}
#endif
