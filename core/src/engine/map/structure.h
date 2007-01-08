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

#ifndef FIFE_MAP_STRUCTURE_H
#define FIFE_MAP_STRUCTURE_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/point.h"

namespace FIFE { namespace map {
	struct grid_info {
		size_t geometry;
		std::string overlay;
		Point size;
		Point shift;
		Point overlay_offset;

		grid_info(size_t geometry, const std::string& overlay, Point size, 
		          Point shift = Point(), Point overlay_offset = Point()) :
			geometry(geometry), overlay(overlay), size(size), shift(shift),
			overlay_offset(overlay_offset) {}
	};

	struct elevation_info {
		size_t numberOfGrids;
		std::vector<grid_info> grids;
		size_t referenceGrid;

		elevation_info(grid_info* begin, grid_info* end, int rg) : 
			numberOfGrids(end - begin), grids(begin, end), referenceGrid(rg) {}

		/* FIXME? */
		elevation_info() {}
	};

	/** Tiny Helper class for Elevation Creation
	 *
	 *  This is a tiny helper class that helps creating
	 *  Elevations of different formats.
	 *
	 *  Currently only Fallout2 Format is supported.
	 */
	elevation_info falloutElevationFormat(const Point& size);
} } // FIFE::map

#endif // FIFE_MAPHEADER_H
