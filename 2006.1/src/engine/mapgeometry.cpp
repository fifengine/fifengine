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

#include "mapgeometry.h"

#include "defaulttilegeometry.h"
#include "defaultobjectgeometry.h"

namespace FIFE {

	GridGeometry** GridGeometry::m_predefined_geometries = 0;

    GridGeometry::GridGeometry(){};
    GridGeometry::~GridGeometry(){};

	GridGeometry* GridGeometry::getGeometryFromId(size_t id ) {
		if( 0 == m_predefined_geometries ) {
			initGeometries();
		}
		return m_predefined_geometries[ id ];
	}

	void GridGeometry::initGeometries() {
		m_predefined_geometries = new GridGeometry*[2];
		m_predefined_geometries[0] = new DefaultTileGeometry();
		m_predefined_geometries[1] = new DefaultObjectGeometry();
		
	}
};


