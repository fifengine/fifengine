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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"
#include "loaders/native/video_loaders/complexanimation_provider.h"
#include "loaders/fallout/video_loaders/frm_provider.h"
#include "loaders/native/video_loaders/animation_provider.h"
#include "loaders/native/video_loaders/image_provider.h"
#include "loaders/native/video_loaders/subimage_provider.h"

#include "renderable_provider.h"

namespace FIFE {

	RenderableProvider::RenderableProvider(const RenderableLocation& location)
	:  m_location(location), m_refcount(0) {
	};

	RenderableProvider::~RenderableProvider() {
	}

	void RenderableProvider::unload() {
		// DUMMY IMPLEMENTATION
	};

	RenderableProviderConstructor* RenderableProviderConstructor::createRenderableProviderConstructor(RenderableProviderType type) {
		switch(type) {
			case ERPT_FRM:
				return new RenderableProviderConstructorTempl<
				            video::loaders::FRMProvider,
				            RenderAble::RT_IMAGE|RenderAble::RT_ANIMATION|RenderAble::RT_UNDEFINED >();

			case ERPT_Animation:
				return new RenderableProviderConstructorTempl<
				            video::loaders::AnimationProvider,
				            RenderAble::RT_ANIMATION >();

			case ERPT_ComplexAnimation:
				return new RenderableProviderConstructorTempl<
			 	           video::loaders::ComplexAnimationProvider,
				            RenderAble::RT_COMPLEX_ANIMATION >();

			case ERPT_SubImage:
				return new RenderableProviderConstructorTempl<
				            video::loaders::SubImageProvider,
				            RenderAble::RT_SUBIMAGE >();
		
			case ERPT_Image:
				return new RenderableProviderConstructorTempl<
				            video::loaders::ImageProvider,
				            RenderAble::RT_IMAGE|RenderAble::RT_UNDEFINED >(); 
		}

		return 0;
	}

};
