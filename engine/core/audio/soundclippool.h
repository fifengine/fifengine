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
 
#ifndef FIFE_SOUNDCLIPPOOL_H_
#define FIFE_SOUNDCLIPPOOL_H_

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/resource/pool.h"

#include "soundclip.h"

namespace FIFE {
	
	/**  Pool for holding sound clips
	 */
	class SoundClipPool: public Pool {
	public:
		/** Default constructor.
		 */
		SoundClipPool(const std::string& name = "SoundPool"): Pool(name) {
		}
	
		/** Destructor.
		 */
		virtual ~SoundClipPool() {}
	
		SoundClip& getSoundClip(unsigned int index)  {
			return dynamic_cast<SoundClip&>(get(index));
		}
	};
}

#endif
