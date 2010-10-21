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

#ifndef FIFE_IMAGE_POOL_H
#define FIFE_IMAGE_POOL_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "image.h"

#include "util/resource/pool.h"

namespace FIFE {
	class Image;

	/**  Pool for holding images
	 */
	class ImagePool: public Pool {
	public:
		/** Default constructor.
		 */
		ImagePool(const std::string& name = "ImagePool"): Pool(name) {
		}

		/** Destructor.
		 */
		virtual ~ImagePool() {}

		void invalidateLoadedImages() {
			std::vector<PoolEntry*>::iterator it;
			for (it = m_entries.begin(); it != m_entries.end(); it++) {
				PoolEntry* entry = *it;
				if( entry->resource) {
					static_cast<Image*>(entry->resource)->invalidate();
				}
			}
		}

		inline Image& getImage(unsigned int index)  {
			return static_cast<Image&>(get(index));
		}
	};

} //FIFE

#endif
