/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_VIDEO_LOADERS_ANIM_PROVIDER_H
#define FIFE_VIDEO_LOADERS_ANIM_PROVIDER_H

// Standard C++ library includes

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/resource/pooled_resource.h"
#include "video/image.h"

namespace FIFE { 
	class Animation;
	class ImagePool;

	class AnimationLoader : public IPooledResourceLoader {
	public:
		AnimationLoader(ImagePool* pool): m_pool(pool) {}
		virtual IResource* loadResource(const ResourceLocation& location);

		Animation* loadAnimation(const ResourceLocation& location);
		Animation* loadAnimation(const std::string& filename) { return loadAnimation(ResourceLocation(filename)); }

	private:
		ImagePool* m_pool;
	};

}
#endif
