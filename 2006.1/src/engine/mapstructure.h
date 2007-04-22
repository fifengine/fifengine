/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#ifndef FIFE_MAPHEADER_H
#define FIFE_MAPHEADER_H
#include "point.h"
#include <vector>
#include <string>

namespace FIFE {

	typedef struct {
		size_t geometry;
		Point size;
		Point shift;
		std::string overlay;
	} s_grid_info;

	typedef struct {
		size_t numberOfTileGrids;
		std::vector<s_grid_info> tileGrids;

		size_t numberOfObjectGrids;
		std::vector<s_grid_info> objectGrids;

	} s_elevation_info;

	class MapStructure {
		public:
			static s_elevation_info falloutElevationFormat(const Point& size);
	};
} // FIFE

#endif // FIFE_MAPHEADER_H
