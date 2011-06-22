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

#ifndef FIFE_IATLAS_LOADER_H
#define FIFE_IATLAS_LOADER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/imagemanager.h"

namespace FIFE {

	class Atlas {
	public:
		Atlas(const std::string& name)
			: m_name(name) {;}
		~Atlas() {;}

		typedef std::map<std::string, Rect> SubimageMap;

		SubimageMap& getSubimages() { return m_subimages; }
		ImagePtr& getImage() { return m_image; }

		void setImage(const ImagePtr& image) { m_image = image; }
	protected:
		std::string m_name;
		SubimageMap m_subimages;
		ImagePtr m_image;
	};

	class IAtlasLoader {
	public:
		virtual ~IAtlasLoader() { };

		/** determines whether the resource is in
		*	the correct format for this loader
		*/
		virtual bool isLoadable(const std::string& filename) = 0;

		/** responsible for loading the atlas
		 * returns a shared pointer to an image resource
		*/
		virtual Atlas* load(const std::string& filename) = 0;
	};
}

#endif
