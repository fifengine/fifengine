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
		virtual void setXShift(int xshift);

		/** Sets the Y shift of the Renderable
		 * @param yshift The Y shift of the Renderable
		 */
		virtual void setYShift(int yshift);

		/** Gets the X shift of the Renderable
		 * @return The X shift of the Renderable
		 */
		virtual int getXShift() const;

		/** Gets the Y shift of the Renderable
		 * @param yshift The Y shift of the Renderable
		 */
		virtual int getYShift() const;

	private:
		// The X shift of the Image
		unsigned int m_xshift;
		// The Y shift of the Image
		unsigned int m_yshift;

	};

}; //FIFE
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
