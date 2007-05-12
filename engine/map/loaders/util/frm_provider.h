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

#ifndef FIFE_MAP_LOADERS_UTIL_FRM_PROVIDER_H
#define FIFE_MAP_LOADERS_UTIL_FRM_PROVIDER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderable_provider.h"


namespace FIFE { namespace map { namespace loaders {
	
	class fallout::AnimatedPalette;
	
	namespace util {

	class FRMProvider : public RenderableProvider {

		public:
			/** Construct from a file encapsulated in a RenderableLocation
			 */
			FRMProvider(const RenderableLocation& location) :
				RenderableProvider(location) {}

			/** Load the FRM and return a RenderAble
			 */
			virtual RenderAble* createRenderable();
			
		private:
			/** Holds fallout palette information. Only need one, but many FRMProviderobjects
			 * are created, so this is static.
			 */
			static fallout::AnimatedPalette* m_palette;
	};
} } } }

#endif // FRM_PROVIDER_H
