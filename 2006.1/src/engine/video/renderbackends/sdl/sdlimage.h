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

#ifndef FIFESDLIMAGE_H
#define FIFESDLIMAGE_H

#include "video/image.h"
#include <SDL_video.h>

namespace FIFE {

	/** The SDL implementation of the @c Image base class
	 */
	class SDLImage : public Image {
		public:
			/** Create a new @c SDLImage from an SDL surface
			 */
			SDLImage(SDL_Surface* surface);

			/** Destructor
			 * Destroy the @c SDLImage and free any associated memory.
			 */
			virtual ~SDLImage();

			/** Render the image on a @c SDLScreen with alpha blending.
			 */
			virtual void render(const Rect& rect, Screen* screen, unsigned char alpha = 255);

			virtual unsigned int getWidth() const;
			virtual unsigned int getHeight() const;

		private:
			SDL_Surface* m_surface;
	};

}

#endif
