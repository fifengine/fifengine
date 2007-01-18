/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team				   *
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

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "exception.h"
#include "util/xmlutil.h"

#include "geometry.h"
#include "hexgeometry.h"
#include "gridgeometry.h"

namespace FIFE { namespace map {

	// The predefined geometries used to be lazily instantiated, but there 
	// shouldn't be any issues with the order of initialization (because there 
	// are no real dependencies). There's still a memory leak though :(
	// Also, instead of making a private static member, I hid stuff in a detail
	// namespace, so that it doesn't clutter the "interface" header.
	
	namespace geometry_detail {
		std::map<size_t,s_geometry_info> info_map;

		// Free function helpers - we avoid adding to the 'private' section of the
		// interface
		
		enum TransformDirection { TO_SCREEN, FROM_SCREEN };
		
		// This could be done with function pointers also, probably...
		Point transform(const Point& p, const Geometry& g, TransformDirection t) {
			switch (t) {
				case TO_SCREEN:
				return g.toScreen(p);

				case FROM_SCREEN:
				return g.fromScreen(p);
			}
			assert(0);
		}
		
		Rect boundingRect(const Rect& r, const Geometry& g, TransformDirection t) {
			// Get x and y values of the transformed rect corners.	
			Point ul = transform(Point(r.x, r.y), g, t);
			Point ur = transform(Point(r.right(), r.y), g, t);
			Point ll = transform(Point(r.x, r.bottom()), g, t);
			Point lr = transform(Point(r.right(), r.bottom()), g, t);

			// For the bounding rect, extract the min and max x and y values.
			int left   = std::min(std::min(std::min(ul.x, ur.x), ll.x), lr.x);
			int top    = std::min(std::min(std::min(ul.y, ur.y), ll.y), lr.y);
			int right  = std::max(std::max(std::max(ul.x, ur.x), ll.x), lr.x);
			int bottom = std::max(std::max(std::max(ul.y, ur.y), ll.y), lr.y);
		
			return Rect(left, top, right - left, bottom - top);
		}
	}

	Geometry* Geometry::createGeometry(size_t id, const Point& mapsize) {
		std::map<size_t,s_geometry_info>::iterator i = geometry_detail::info_map.find(id);
		if( i == geometry_detail::info_map.end() ) {
			throw InvalidFormat(std::string("unknown geometry id: ")
				+ boost::lexical_cast<std::string>(id));
		}

		if( i->second.geometry == "HEXAGONAL" ) {
			return new HexGeometry(i->second, mapsize);
		}

		if( i->second.geometry == "RECTANGULAR" ) {
			return new GridGeometry(i->second, mapsize);
		}

		throw InvalidFormat(std::string("unknown geometry: ")
			+ boost::lexical_cast<std::string>(i->second.geometry));
		return 0;
	}

	void Geometry::registerGeometry(const s_geometry_info& g) {
		geometry_detail::info_map[g.id] = g;
	}

	// Default implementations.
	
	Point Geometry::directionToScreen(size_t dir) const {
		return toScreen(directionToGrid(dir)) - toScreen(Point());
	}
	
	Rect Geometry::screenBoundingRect(const Rect& gridRect) const {
		return geometry_detail::boundingRect(gridRect, *this, geometry_detail::TO_SCREEN);
	}

	Rect Geometry::gridBoundingRect(const Rect& screenRect) const {
		return geometry_detail::boundingRect(screenRect, *this, geometry_detail::FROM_SCREEN);
	}


	s_geometry_info s_geometry_info::load(TiXmlElement* e) {
		xmlutil::assertElement(e, "geometry" );
		return s_geometry_info(xmlutil::queryElement<size_t>(e,"id"),
		                       xmlutil::queryElement<std::string>(e,"type"),
		                       xmlutil::queryElement<Point>(e,"size"),
		                       xmlutil::queryElement<Point>(e,"transform"),
		                       xmlutil::queryElement<Point>(e,"offset"),
		                       xmlutil::queryElement<int>(e,"flags"));
	}

} };
