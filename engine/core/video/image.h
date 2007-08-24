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

#ifndef FIFE_VIDEO_IMAGE_H
#define FIFE_VIDEO_IMAGE_H

// Standard C++ library includes

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/resource/pooled_resource.h"

namespace FIFE {
	class Rect;

	/** Base Class for Images.
	 */
	class Image : public IPooledResource {
		public:
			/** Constructor.
			 */
			Image(SDL_Surface* surface);

			/** Destructor.
			 */
			virtual ~Image();

			/** Get the surface used by this image
			 * @return pointer to used surface
			 */
			SDL_Surface* getSurface();

			/** Sets the X shift of the Image.
			 * @param xshift The X shift of the Image.
			 */
			void setXShift(int xshift);

			/** Gets the X shift of the Image.
			 * @return The X shift of the Image.
			 */
			int getXShift() const;

			/** Sets the Y shift of the Image.
			 * @param yshift The Y shift of the Image.
			 */
			void setYShift(int yshift);

			/** Gets the Y shift of the Image.
			 */
			int getYShift() const;

			/** Renders itself to the Screen at the rectangle rect.
			 *
			 * @param rect The position and clipping where to draw this image to.
			 * @param screen The Screen as it is created by the RenderBackend. Draw self on this.
			 * @param alpha The alpha value, with which to draw self.
			 * @see RenderBackend, Screen
			 */
			virtual void render(const Rect& rect, SDL_Surface* screen, unsigned char alpha = 255) = 0;

			/** Returns the @b maximum width occupied by this image.
			 * This should return the maximum width that could be covered by the
			 * image.
			 */
			virtual unsigned int getWidth() const = 0;

			/** Returns the @b maximum height occupied by this image.
			 */
			virtual unsigned int getHeight() const = 0;

			/** Enable or disable the alpha 'optimizing' code
			 *
			 * @param optimize Wether the image shall be analysed for 'fake' alpha images.
			 * Only implemented by and useful for the SDL backend at the moment.
			 */
			virtual void setAlphaOptimizerEnabled(bool) {};

			void addRef() { m_refcount++; };
			void decRef() { m_refcount--; };
			unsigned int getRefCount() { return m_refcount; };

		protected:
			// The SDL Surface used.
			SDL_Surface* m_surface;
			// The X shift of the Image
			int m_xshift;
			// The Y shift of the Image
			int m_yshift;
			// Reference count of this image, used e.g. by animations
			unsigned int m_refcount;
	};

}

#endif
