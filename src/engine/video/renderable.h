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

#ifndef FIFERENDERABLE_H
#define FIFERENDERABLE_H
#include <boost/intrusive_ptr.hpp>

namespace FIFE {

	class Screen;
	class Rect;
//	class RenderableProvider;
	class Image;
	class Animation;

/*
	void intrusive_ptr_add_ref(RenderableProvider* p);
	void intrusive_ptr_release(RenderableProvider* p);
	typedef ::boost::intrusive_ptr<RenderableProvider> RenderableProviderPtr;
*/

	/** The Abstract Base Class for anything renderable
	 *
	 * The RenderAble Class is used as a basis for handling any drawing operations of
	 * loadable graphics. Use the ImageCache to load renderables. This encapsulates 
	 * loading and creation of Graphics completely. The RenderAble class is then 
	 * subclassed by Image, Animation and ComplexAnimation. The later are yet somewhat
	 * incomplete.
	 *
	 * The Image subclass itself is only a stub for subclasses belonging to the RenderBackend
	 * That is. An Image is always either an SDLImage (thus basically a SDL surface) or and
	 * GLImage (and thus basically a texture) -- this was all done to encapsulate the backend
	 * as good as possible. Loading Classes/Functions will have to use calls into the backend
	 * to create Images and thus automatically the correct type of Image (SDL,GL) is created.
     * Animations and ComplexAnimations are different, these are containers of Images and thus
	 * don't need any backend specific sub-class. Still it is good to have the as a subclass to
	 * a renderable, since this means, they can be used like images, and so providing, for example
	 * the possibility to have animations in the GUI without any extra code (that's the plan...)
	 * 
	 * @see RenderBackend
	 * @see Image, Animation, ComplexAnimation
	 * @see ImageCache 
	 */
	class RenderAble {
		public:
			typedef enum {
				RT_IMAGE,
				RT_ANIMATION,
				RT_COMPLEX_ANIMATION,
				RT_ANY
			} RenderableTypes;

			/** Constructor
			 * Called by derived classes with the respective RenderableTypes value.
			 */
			RenderAble(RenderableTypes);

			/** Destructor
			 */
			virtual ~RenderAble();

			/** Render itself to the Screen at the rectangle rect.
			 * @param rect The position and clipping where to draw this renderable to.
			 * @param screen The Screen as it is created by the RenderBackend. Draw self on this.
			 * @param alpha The alpha value, with which to draw self.
			 * @see RenderBackend, Screen
			 */
			virtual void render(const Rect& rect, Screen* screen, unsigned char alpha = 255) = 0;

			/** Return the \b maximum width occupied by this renderable.
			 * This should return the maximum width that could be covered by the
			 * renderable. For an Image this is quite clear (though possible shifts have to be
			 * taken into account. For an Animation this however means, that it should return
			 * the maximum width from all frames.
			 */
			virtual unsigned int getWidth() const = 0;
			/** Return the \b maximum height occupied by this renderable.
			 * @see getWidth()
			 */
			virtual unsigned int getHeight() const = 0;

			/** Set the X shift of the renderable
			 * @note The shifts @b need more documentation. 
			 */
			virtual void setXShift(int xshift) =0;
			/** Set the Y shift of the renderable
			 */
			virtual void setYShift(int yshift) =0;
			/** Get the X shift of the renderable
			 */
			virtual int getXShift() const =0;
			/** Get the Y shift of the renderable
			 */
			virtual int getYShift() const =0;

//			bool hasProvider() const;
//			void setProvider(RenderableProviderPtr p);
//			RenderableProviderPtr getProvider() const { return m_provider; };

			/** Return the type of renderable as set in the Constructor
			 */
			RenderableTypes getType() const { return m_type; };
			/*virtual Image* toImage() { return 0; };
			virtual Animation* toAnimation() { return 0; };*/
		protected:
			RenderableTypes m_type;
//			RenderableProviderPtr m_provider;

//		friend class RenderableProvider;
	};
/*
	inline
	bool RenderAble::hasProvider() const {
		return m_provider != 0;
	};

	inline
	void RenderAble::setProvider(RenderableProviderPtr p) {
		m_provider = p;
	};
*/
}

#endif
