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

#ifndef FIFE_VIDEO_SCREEN_H
#define FIFE_VIDEO_SCREEN_H

// Standard C++ library includes

// 3rd party library includes
#include <guichan/graphics.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	/** Abstract Screen.
	 *
	 * A derived class needs to be implemented by each renderbackend.
	 *
	 * @note This class is completely empty.
	 *
	 * See any of the derived classes for more details:
	 *
	 * @see GLScreen
	 * @see SDLScreen
	 */
	class Screen {
		public:
			/** Destructor.
			 */
			virtual ~Screen() {};

			/** Gets the width of the screen.
			 *
			 * @return Width of the screen.
			 */
			virtual unsigned int getWidth() const = 0;

			/** Gets the height of the screen.
			 *
			 * @return Height of the screen.
			 */
			virtual unsigned int getHeight() const = 0;
	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
