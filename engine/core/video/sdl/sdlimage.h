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

#ifndef FIFE_VIDEO_RENDERBACKENDS_SDL_SDLIMAGE_H
#define FIFE_VIDEO_RENDERBACKENDS_SDL_SDLIMAGE_H

// Standard C++ library includes

// 3rd party library includes
#include <SDL_video.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/image.h"

namespace FIFE {

	/** The SDL implementation of the @c Image base class.
	 */
	class SDLImage : public Image {
		public:
			/** Contructor.
			 *
			 * Creates a new @c SDLImage from an SDL surface
			 *
			 * @note Takes ownership of the SDL Surface
			 * @param surface The SDL surface from which to create the SDLImage.
			 */
			SDLImage(SDL_Surface* surface);

			/** Destructor.
			 *
			 * Destroys the @c SDLImage and frees any associated memory.
			 */
			virtual ~SDLImage();

			/** Renders the image on a @c SDLScreen with alpha blending.
			 *
			 * @param rect The rect of the image to render.
			 * @param screen The screen on which to render the image.
			 * @param alpha The level of transparency, opaque by default.
			 */
			virtual void render(const Rect& rect, SDL_Surface* screen, unsigned char alpha = 255);

			/** Gets the width of the image.
			 *
			 * @return Width of the image.
			 */
			virtual unsigned int getWidth() const;
			/** Gets the height of the image.
			 *
			 * @return Height of the image.
			 */
			virtual unsigned int getHeight() const;

			/** Enable or disable the alpha 'optimizing' code
			 *
			 * @param optimize Wether the image shall be analysed for 'fake' alpha images.
			 */
			virtual void setAlphaOptimizerEnabled(bool optimize);

		private:
			// Call this before rendering
			void finalize();
			/** SDL Alpha Optimizer
			 * This tries to convert an image with a fake alpha channel
			 * to an RGB image when the channel can be reasonably be replaced
			 * by an colorkey.
			 */
			SDL_Surface* optimize(SDL_Surface* surface);

			// SDLSurface used to create the SDLImage.
			Uint8 m_last_alpha;
			// Is the surface already optimized for rendering
			bool m_finalized;
			// Wether to try to optimize alpha out ...
			bool m_optimize_alpha;
	};

}

#endif
