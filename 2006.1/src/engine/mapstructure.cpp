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

#include "mapstructure.h"
#include "mapgeometry.h"


namespace FIFE {

	s_elevation_info MapStructure::falloutElevationFormat( const Point& size ) {
		s_elevation_info elev;
		elev.numberOfTileGrids = 2;
		elev.numberOfObjectGrids = 1;

		// Floor
		s_grid_info grid;
		grid.shift = Point();
		grid.size = size;
		grid.geometry = GridGeometry::FalloutTileGeometry;
		grid.overlay = "content/gfx/tiles/tile_outline.png";

		elev.tileGrids.push_back( grid );
		// Roof
		grid.shift = Point(0,96);
		elev.tileGrids.push_back( grid );

		// Objects
		grid.shift = Point();
		grid.size = size*2;
		grid.geometry = GridGeometry::FalloutObjectGeometry;
		grid.overlay = "art/intrface/msef000.frm";
		
		elev.objectGrids.push_back( grid );
		return elev;
	}
}
