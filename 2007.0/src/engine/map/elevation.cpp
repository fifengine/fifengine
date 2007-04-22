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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "debugutils.h"
#include "exception.h"

#include "elevation.h"
#include "grid.h"
#include "geometry.h"

namespace FIFE { namespace map {
	Elevation::Elevation(const elevation_info& structure) 
		: AttributedClass("map_elevation"),
		m_reference_grid(0) {

		setNumGrids(structure.numberOfGrids);
		for (size_t i = 0; i != structure.numberOfGrids; ++i) {
			Grid *grid = new Grid(structure.grids[i].size,
			                      structure.grids[i].geometry);
			grid->setShift(structure.grids[i].shift);
			grid->setOverlayImage(structure.grids[i].overlay);
			grid->setAttribute("overlay-offset", structure.grids[i].overlay_offset);

			setGrid(i, grid);
		}
		setReferenceGrid(structure.referenceGrid);
	}

	// XXX Why are we storing pointers?
	// Consider also boost::ptr_vector
	Elevation::~Elevation() {
		cleanup();
	}

	void Elevation::cleanup() {
		type_grids::const_iterator end = m_grids.end();
		for (type_grids::iterator i = m_grids.begin(); i != end; ++i) {
			delete *i;
		}
	}

	size_t Elevation::getNumGrids() const {
		return m_grids.size();
	}

	void Elevation::setNumGrids(size_t n) {
		m_grids.resize(n);
	}

	void Elevation::setGrid(size_t idx, Grid* grid) {
		m_grids[idx] = grid;
	}

	Grid* Elevation::getGrid(size_t idx) {
		return m_grids.at(idx);
	}

	void Elevation::setReferenceGrid(size_t grid) {
		m_reference_grid = grid;
	}

	Grid* Elevation::getReferenceGrid() {
		return getGrid(m_reference_grid);
	}

	Point Elevation::centerOfMass() {
		size_t n = 0;
		Point p;

		// Sanity checks.
		Grid* ref_grid = getReferenceGrid();
		if( ref_grid == 0 )
			return p;

		Geometry* ref_geo = ref_grid->getGeometry();
		if( ref_geo == 0 )
			return p;


		type_grids::iterator end = m_grids.end();
		for (type_grids::iterator i = m_grids.begin(); i != end; ++i) {
			Point pos;
			Geometry *geo = (*i)->getGeometry();
			if( geo == 0 )
				continue;

			for(pos.x=0; pos.x != (*i)->getSize().x; ++pos.x) {
				for(pos.y=0; pos.y != (*i)->getSize().y; ++pos.y) {
					if( (*i)->getTileImage(pos) ) {
						p += ref_geo->fromScreen(geo->toScreen(pos));
						++n;
					}
				}
			}
		}

		return p/n;
	}

} } //FIFE::map
