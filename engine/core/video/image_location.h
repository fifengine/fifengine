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

#ifndef FIFE_VIDEO_IMAGE_LOCATION_H
#define FIFE_VIDEO_IMAGE_LOCATION_H

// Standard C++ library includes
#include <string>
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/image.h"
#include "util/resource/resource_location.h"

namespace FIFE {

	/** Contains information about the Location of a image.
	 *
	 *  This class is used to give ResourceProvider the information
	 *  where to find the data. 
	 *
	 */
	class ImageLocation: public ResourceLocation {
	public:
		/** Constructor.
		 */
		ImageLocation(const std::string& filename);

		/** Sets the X shift of the Image.
		 * @param xshift The X shift of the Image
		 */
		virtual void setXShift(int xshift) { m_xshift = xshift; }

		/** Gets the X shift of the Image
		 * @return The X shift of the Image
		 */
		virtual int getXShift() const { return m_xshift; }

		/** Sets the Y shift of the Image
		 * @param yshift The Y shift of the Image
		 */
		virtual void setYShift(int yshift) { m_yshift = yshift; }

		/** Gets the Y shift of the Image
		 * @param yshift The Y shift of the Image
		 */
		virtual int getYShift() const { return m_yshift; }

		/** Sets the width of the Image. Used e.g. with subimages
		 */
		virtual void setWidth(unsigned int width) { m_width = width; }

		/** Gets the width of the Image. Used e.g. with subimages
		 */
		virtual unsigned int getWidth() const { return m_width; }

		/** Sets the height of the Image. Used e.g. with subimages
		 */
		virtual void setHeight(unsigned int height) { m_height = height; }

		/** Gets the height of the Image. Used e.g. with subimages
		 */
		virtual unsigned int getHeight() const { return m_height; }

		/** Sets the parent source of the Image. Used e.g. with subimages
		 */
		virtual void setParentSource(Image* image) { m_parent_image = image; }

		/** Gets the parent source of the Image. Used e.g. with subimages
		 */
		virtual Image* getParentSource() const { return m_parent_image; }

		virtual bool operator ==(const ResourceLocation& loc) const;
		virtual bool operator <(const ResourceLocation& loc) const;
		virtual ResourceLocation* clone() const;

	private:
		unsigned int m_xshift;
		unsigned int m_yshift;
		unsigned int m_width;
		unsigned int m_height;
		Image* m_parent_image;
	};

}; //FIFE
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
