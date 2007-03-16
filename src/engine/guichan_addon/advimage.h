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

#ifndef FIFE_GUICHAN_ADDON_ADVIMAGE_H
#define FIFE_GUICHAN_ADDON_ADVIMAGE_H

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <guichan.hpp>
#if GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6
#include "video/gui/gcnfifeimage.h"
#endif

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {
	class Animation;
}

namespace gcn {

#if GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6
	class AdvImage : public FIFE::GCNImage {
#else
	class AdvImage : public Image {
#endif
		public:
#if GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6
			AdvImage();
			void loadFromFile(const std::string & filename);
			void loadFromCache(const size_t id);
#endif
			AdvImage(const std::string& filename);
			~AdvImage();

			void setAnimActive(bool active);
			void setAnimDirection(bool forward);
		protected:
			FIFE::Animation* getImageAsAnimation();
	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
