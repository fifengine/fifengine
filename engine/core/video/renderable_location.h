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

#ifndef FIFE_VIDEO_RENDERABLE_LOCATION_H
#define FIFE_VIDEO_RENDERABLE_LOCATION_H

// Standard C++ library includes
#include <string>
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderable.h"
#include "util/resource/resource_location.h"

namespace FIFE {

	/** Contains information about the Location of a Renderable.
	 *
	 *  This class is used to give ResourceProvider the information
	 *  where to find the data. 
	 *
	 */
	class RenderableLocation: public ResourceLocation {
	public:
		/** Constructor.
		 */
		RenderableLocation(const std::string& filename);

		/** Sets the X shift of the Renderable.
		 * @param xshift The X shift of the Renderable
		 */
		virtual void setXShift(int xshift) { m_xshift = xshift; }

		/** Gets the X shift of the Renderable
		 * @return The X shift of the Renderable
		 */
		virtual int getXShift() const { return m_xshift; }

		/** Sets the Y shift of the Renderable
		 * @param yshift The Y shift of the Renderable
		 */
		virtual void setYShift(int yshift) { m_yshift = yshift; }

		/** Gets the Y shift of the Renderable
		 * @param yshift The Y shift of the Renderable
		 */
		virtual int getYShift() const { return m_yshift; }

		/** Sets the width of the Renderable. Used e.g. with subimages
		 */
		virtual void setWidth(unsigned int width) { m_width = width; }

		/** Gets the width of the Renderable. Used e.g. with subimages
		 */
		virtual unsigned int getWidth() const { return m_width; }

		/** Sets the height of the Renderable. Used e.g. with subimages
		 */
		virtual void setHeight(unsigned int height) { m_height = height; }

		/** Gets the height of the Renderable. Used e.g. with subimages
		 */
		virtual unsigned int getHeight() const { return m_height; }

		/** Sets the parent source of the Renderable. Used e.g. with subimages
		 */
		virtual void setParentSource(Renderable* renderable) { m_parent_renderable = renderable; }

		/** Gets the parent source of the Renderable. Used e.g. with subimages
		 */
		virtual Renderable* getParentSource() const { return m_parent_renderable; }

	private:
		unsigned int m_xshift;
		unsigned int m_yshift;
		unsigned int m_width;
		unsigned int m_height;
		Renderable* m_parent_renderable;

	};

}; //FIFE
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
