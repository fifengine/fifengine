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

#ifndef FIFE_VIDEO_RENDERABLE_H
#define FIFE_VIDEO_RENDERABLE_H

// Standard C++ library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include <boost/intrusive_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	class Screen;
	class Rect;
	class Image;
	class Animation;

	/** The Abstract Base Class for anything renderable.
	 *
	 * The RenderAble Class is used as a basis for handling any drawing operations of
	 * loadable graphics. Use the ImageCache to load renderables. This encapsulates 
	 * loading and creation of Graphics completely. The RenderAble class is then 
	 * subclassed by Image and Animation. The later are yet somewhat incomplete.
	 *
	 * The Image subclass itself is only a stub for subclasses belonging to the RenderBackend
	 * That is, an Image is always either an SDLImage (thus basically a SDL surface) or and
	 * GLImage (and thus basically a texture) -- this was all done to encapsulate the backend
	 * as good as possible. Loading Classes/Functions will have to use calls into the backend
	 * to create Images and thus automatically the correct type of Image (SDL,GL) is created.
	 * Animations are different, these are containers of Images and thus
	 * don't need any backend specific sub-class. Still it is good to have the as a subclass to
	 * a renderable, since this means, they can be used like images, and so providing, for example
	 * the possibility to have animations in the GUI without any extra code (that's the plan...)
	 * 
	 * @see RenderBackend
	 * @see Image, Animation
	 * @see ImageCache 
	 */
	class RenderAble {
		public:
			typedef enum {
				RT_UNDEFINED          = 1,
				RT_IMAGE              = 2,
				RT_ANIMATION          = 4,
				RT_SUBIMAGE           = 16,
				RT_ANY                = 31
			} RenderableTypes;

			/** Constructor.
			 *
			 * Called by derived classes with the respective RenderableTypes value.
			 */
			RenderAble(RenderableTypes);

			/** Destructor.
			 */
			virtual ~RenderAble();

			/** Renders itself to the Screen at the rectangle rect.
			 *
			 * @param rect The position and clipping where to draw this renderable to.
			 * @param screen The Screen as it is created by the RenderBackend. Draw self on this.
			 * @param alpha The alpha value, with which to draw self.
			 * @see RenderBackend, Screen
			 */
			virtual void render(const Rect& rect, Screen* screen, unsigned char alpha = 255) = 0;

			/** Returns the @b maximum width occupied by this renderable.
			 * This should return the maximum width that could be covered by the
			 * renderable. For an Image this is quite clear (though possible shifts have to be
			 * taken into account). For an Animation this however means, that it should return
			 * the maximum width from all frames.
			 */
			virtual unsigned int getWidth() const = 0;
			/** Returns the @b maximum height occupied by this renderable.
			 *
			 * @see getWidth()
			 */
			virtual unsigned int getHeight() const = 0;

			/** Sets the X shift of the renderable.
			 *
			 * @param xshift The X shift of the renderable.
			 * @note The shifts @b need more documentation. 
			 */
			virtual void setXShift(int xshift) =0;
			/** Sets the Y shift of the renderable.
			 *
			 * @param yshift The Y shift of the renderable.
			 */
			virtual void setYShift(int yshift) =0;
			/** Gets the X shift of the renderable.
			 *
			 * @return The X shift of the renderable.
			 */
			virtual int getXShift() const =0;
			/** Gets the Y shift of the renderable.
			 *
			 * @return The Y shift of the renderable.
			 */
			virtual int getYShift() const =0;

			/** Returns the type of renderable as set in the Constructor.
			 *
			 * @return The type of RenderAble.
			 */
			RenderableTypes getType() const { return m_type; };
		protected:
			/// The type of RenderAble.
			RenderableTypes m_type;
	};
}

#endif
