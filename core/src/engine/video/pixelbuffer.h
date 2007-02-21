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

#ifndef FIFE_PIXELBUFFER_H
#define FIFE_PIXELBUFFER_H

// Standard C++ library includes
#include <boost/shared_ptr.hpp>

// 3rd party library includes
#include <SDL.h>

// FIFE includes

namespace FIFE {

	/** Extremly simple SDL_Surface wrapper.
	 *
	 * @see Image
	 */
	class PixelBuffer  {
		public:
			/** Constructor. from SDL_Surface
			 */
			PixelBuffer(SDL_Surface* surface);

			/** Destructor.
			 */
			~PixelBuffer();

			/** Get the SDL_Surface
			 */
			SDL_Surface* getSurface() const;

		protected:
			SDL_Surface* m_surface;
	};
	typedef boost::shared_ptr<PixelBuffer> PixelBufferPtr;

}

#endif
