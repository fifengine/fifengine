/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

#ifndef FIFE_MAP_GEOMETRY_H
#define FIFE_MAP_GEOMETRY_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/point.h"
#include "util/rect.h"

class TiXmlElement;

namespace FIFE { namespace model {

	struct s_geometry_info {
		s_geometry_info() {}

		s_geometry_info(size_t _id, const std::string& _g,
		                const Point& _size,
		                const Point& _transform,
		                const Point& _offset,
		                int _flags)
			: id(_id), geometry(_g), size(_size), transform(_transform),
			  offset(_offset), flags(_flags) {}

		static s_geometry_info load(TiXmlElement* e);

		// id
		size_t id;

		// geometry class
		std::string geometry;

		// base size
		Point size;

		// topmost left border intersection point
		Point transform;

		// extra offset
		Point offset;

		int flags;
	};

	/** The Abstract base class of a tile geometry
	 */
	class Geometry {
		public:
			enum {
				FalloutTileGeometry = 0,
				FalloutObjectGeometry = 1
			};

			enum {
				ShiftXAxis = 1,
				ShiftYAxis = 2
			};


			virtual ~Geometry() {}

			static Geometry* createGeometry(const s_geometry_info& data, const Point& mapsize);

			/** Get the raw geometry data for this geometry; used to reconstitute
			 *  an xml map file when saving. Maybe this method should be reconsidered?
			 */
			const s_geometry_info& getInfo();

			/** Calculate screen position of the upper left corner of the grid 
			 *  position
			 *
			 *  (XXX: Should this be the 'upper left corner'? Does that make sense
			 *  for a hex tile e.g.? Maybe it should be to the center, or to some
			 *  "natural reference point" :s )
			 */
			virtual Point toScreen(const Point& gridPos) const = 0;

			/** Calculate the grid position from an screen position
			 */
			virtual Point fromScreen(const Point& screenPos) const = 0;

			/** Get the basic screen size of a single grid cell
			 */
			virtual Point baseSize() const = 0;

			/** Calculate a screen rectangle, that completely encloses a grid 
			 *  rectangle
			 */
			virtual Rect screenBoundingRect(const Rect& gridRect) const;

			/** Calculate a grid rectangle, that completely encloses a screen 
			 *  rectangle
			 */
			virtual Rect gridBoundingRect(const Rect& screenRect) const;

			/** Calculate Screen Offset of a given Direction
			 */
			virtual Point directionToScreen(size_t dir) const;

			/** Calculate Grid Offset of a given Direction
			 */
			virtual Point directionToGrid(size_t dir, const Point& at = Point()) const  = 0; 

			/** Get Number of Directions
			 */
			virtual size_t getNumDirections() const = 0;

			/** Get the movement costs for adjacent nodes
			 *  Necessary for path finding algorithm.
			 *  \return A array of positive floats, size equal to getNumDirections()
			 */
			virtual const float* getAdjacentCosts() const = 0;

		private:

			s_geometry_info info;	
	};


} } // FIFE::model
#endif
