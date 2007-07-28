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

#include "renderable.h"

namespace FIFE {

	/** Base Class for Images.
	 *
	 * @see Renderable
	 */
	class Image : public Renderable, public IPooledResource {
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
			virtual SDL_Surface* getSurface();

			/** Sets the X shift of the Image.
			 * @param xshift The X shift of the Image.
			 */
			virtual void setXShift(int xshift);

			/** Sets the Y shift of the Image.
			 * @param yshift The Y shift of the Image.
			 */
			virtual void setYShift(int yshift);

			/** Gets the X shift of the Image.
			 * @return The X shift of the Image.
			 */
			virtual int getXShift() const;

			/** Gets the Y shift of the Image.
			 * @param yshift The Y shift of the Image.
			 */
			virtual int getYShift() const;
		
		protected:
			// The SDL Surface used.
			SDL_Surface* m_surface;
			// The X shift of the Image
			int m_xshift;
			// The Y shift of the Image
			int m_yshift;
	};

}

#endif
