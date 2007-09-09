/***************************************************************************
 *   Copyright (C) 2006 by the FIFE Team                                   *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef GRIDGEOMETRY_H
#define GRIDGEOMETRY_H

#include "point.h"
#include "rect.h"

namespace FIFE {

	//typedef std::vector<std::pair<int,int> > LineList;

	/** The Abstract base class of a tile geometry
	 */
	class GridGeometry {
		public:
			enum {
				FalloutTileGeometry = 0,
				FalloutObjectGeometry = 1
			};

			/** Constructor:
			 */
			GridGeometry();

			/** Destructor
			 */
			virtual ~GridGeometry();

			static GridGeometry* getGeometryFromId(size_t id);

			/** Calculate screen position of the upper left corner of the grid position
			 */
			virtual Point toScreen(const Point& gridPos) = 0;

			/** Calculate the grid position from an screen position
			 */
			virtual Point fromScreen(const Point& screenPos) = 0;

			/** Get the basic screen size of a single grid cell
			 */
			virtual Point baseSize() = 0;

			/** Calculate a screen rectangle, that completely encloses a grid rectangle
			 */
			virtual Rect screenBoundingRect( const Rect& gridRect ) = 0;

			/** Calculate a grid rectangle, that completely encloses a screen rectangle
			 */
			virtual Rect gridBoundingRect( const Rect& screenRect ) = 0;

		private:
			static void initGeometries();
			static GridGeometry** m_predefined_geometries;
	};
};
#endif
