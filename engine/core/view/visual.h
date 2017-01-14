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

#ifndef FIFE_VIEW_VISUAL_H
#define FIFE_VIEW_VISUAL_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/ivisual.h"
#include "util/math/angles.h"
#include "util/structures/rect.h"
#include "video/animation.h"
#include "video/color.h"


namespace FIFE {
	class Object;
	class Instance;
	class Action;
	class Image;

	class OverlayColors {
	public:
		/** Constructors
		 */
		OverlayColors();
		OverlayColors(ImagePtr image);
		OverlayColors(AnimationPtr animation);

		/** Destructor
		 */
		~OverlayColors();

		void setColorOverlayImage(ImagePtr image);
		ImagePtr getColorOverlayImage();
		void setColorOverlayAnimation(AnimationPtr animation);
		AnimationPtr getColorOverlayAnimation();
		void changeColor(const Color& source, const Color& target);
		const std::map<Color, Color>& getColors();
		void resetColors();

	private:
		std::map<Color, Color> m_colorMap;
		ImagePtr m_image;
		AnimationPtr m_animation;
	};

	/** Base class for all 2 dimensional visual classes
	 * Visual classes are extensions to visualize the stuff in model (e.g. instances)
	 * The reason why its separated is to keep model view-agnostic, so that we could
	 * have e.g. 3d, 2d and character based visualizations to the same data
	 */
	class Visual2DGfx: public IVisual {
	public:
		/** Destructor
		 */
		virtual ~Visual2DGfx();

	protected:
		/** Constructor
		 */
		Visual2DGfx();
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
		void addStaticImage(uint32_t angle, int32_t image_index);

		/** Returns closest matching static image for given angle
		 * @return id for static image
		 */
		int32_t getStaticImageIndexByAngle(int32_t angle);

		/** Adds new static color overlay with given angle (degrees).
		 */
		void addStaticColorOverlay(uint32_t angle, const OverlayColors& colors);

		/** Returns closest matching static color overlay for given angle
		 * @return pointer to OverlayColor class
		 */
		OverlayColors* getStaticColorOverlay(int32_t angle);

		/** Removes a static color overlay with given angle (degrees).
		 */
		void removeStaticColorOverlay(int32_t angle);

		/** Returns closest matching image angle for given angle
		 * @return closest matching angle
		 */
		int32_t getClosestMatchingAngle(int32_t angle);

		/** Returns list of available static image angles for this object
		 */
		void getStaticImageAngles(std::vector<int32_t>& angles);

		/** Indicates if there exists a color overlay.
		 */
		bool isColorOverlay() { return !m_colorOverlayMap.empty(); }

	private:
		/** Constructor
		 */
		ObjectVisual();

		type_angle2id m_angle2img;
		
		typedef std::map<uint32_t, OverlayColors> AngleColorOverlayMap;
		AngleColorOverlayMap m_colorOverlayMap;
		type_angle2id m_map;
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

		/** Sets transparency value for object to be visualized
		 *  @param transparency set the transparency
		 */
		void setTransparency(uint8_t transparency);

		/** Gets current transparency value (0-255)
		 *  @return current transparency value
		 */
		uint8_t getTransparency();

		/** Sets visibility value for object to be visualized
		 *  @param visible is object visible or not
		 */
		void setVisible(bool visible);

		/** Is instance visible or not
		 *  @return is instance visible or not
		 */
		bool isVisible();

		/** Sets stack position of the instance
		 *  Stack position is used to define the order in which instances residing
		 *  in the same location are drawn
		 *  @param stackposition new stack position
		 */
		void setStackPosition(int32_t stackposition);

		/** Gets current stack position of instance
		 *  @return current stack position
		 */
		int32_t getStackPosition();

	private:
		/** Constructor
		 */
		InstanceVisual();
		uint8_t m_transparency;
		bool m_visible;
		int32_t m_stackposition;
		Instance* m_instance;
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
		void addAnimation(uint32_t angle, AnimationPtr animationptr);

		/** Gets index to animation closest to given angle
		 * @return animation index, -1 if no animations available
		 */
		AnimationPtr getAnimationByAngle(int32_t angle);

		/** Adds new animation overlay with given angle (degrees) and order
		 */
		void addAnimationOverlay(uint32_t angle, int32_t order, AnimationPtr animationptr);

		/** Gets map with animations closest to given angle
		 * @return animation map
		 */
		std::map<int32_t, AnimationPtr> getAnimationOverlay(int32_t angle);

		/** Removes animation overlay with given angle (degrees) and order
		 */
		void removeAnimationOverlay(uint32_t angle, int32_t order);

		/** Adds new color overlay with given angle (degrees) and colors.
		 * Note: Works only for single animations not for AnimationOverlays. (order is missing)
		 */
		void addColorOverlay(uint32_t angle, const OverlayColors& colors);

		/** Gets OverlayColors for given angle (degrees).
		 * @return pointer to OverlayColors or Null
		 */
		OverlayColors* getColorOverlay(int32_t angle);

		/** Removes color overlay with given angle (degrees).
		 */
		void removeColorOverlay(int32_t angle);

		/** Adds new color overlay with given angle (degrees), order and colors.
		 * Note: Works only for AnimationOverlays.
		 */
		void addColorOverlay(uint32_t angle, int32_t order, const OverlayColors& colors);

		/** Gets OverlayColors for given angle (degrees) and order.
		 * @return pointer to OverlayColors or Null
		 */
		OverlayColors* getColorOverlay(int32_t angle, int32_t order);

		/** Removes color overlay with given angle (degrees) and order.
		 */
		void removeColorOverlay(int32_t angle, int32_t order);

		/** Returns list of available angles for this Action
		 */
		void getActionImageAngles(std::vector<int32_t>& angles);

		/** Convertes animations and optional color overlay to default animation overlay.
		 * The default order value for both is 0. The old data remain, so if you remove the animation overlay
		 * the old plain animations and colors be used again.
		 */
		void convertToOverlays(bool color);

		/** Returns true if it exists a animation overlay, otherwise false.
		 */
		bool isAnimationOverlay() { return !m_animationOverlayMap.empty(); }

		/** Returns true if it exists a color overlay, otherwise false.
		 */
		bool isColorOverlay() { return !m_colorOverlayMap.empty() || !m_colorAnimationOverlayMap.empty(); }

	private:
		/** Constructor
		 */
		ActionVisual();

		// animations associated with this action
		typedef std::map<uint32_t, AnimationPtr> AngleAnimationMap;
		AngleAnimationMap m_animation_map;

		typedef std::map<uint32_t, std::map<int32_t, AnimationPtr> > AngleAnimationOverlayMap;
		AngleAnimationOverlayMap m_animationOverlayMap;

		typedef std::map<uint32_t, OverlayColors> AngleColorOverlayMap;
		AngleColorOverlayMap m_colorOverlayMap;

		typedef std::map<uint32_t, std::map<int32_t, OverlayColors> > AngleColorAnimationOverlayMap;
		AngleColorAnimationOverlayMap m_colorAnimationOverlayMap;
		//need this map to use the getIndexByAngle() function in angles.h
		type_angle2id m_map;
	};

}
#endif
