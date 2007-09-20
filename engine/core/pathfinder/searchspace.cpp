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

#include "searchspace.h"

namespace FIFE {

	SearchSpace::SearchSpace(const int upperX, const int upperY, const int lowerX, const int lowerY) 
	:m_upperX(upperX), m_upperY(upperY), m_lowerX(lowerX), m_lowerY(lowerY) {
	}

	bool SearchSpace::isInSearchSpace(const Location& location) const {
		ModelCoordinate coordinates = location.getLayerCoordinates();
		if(coordinates.x > m_lowerX && coordinates.x < m_upperX 
			&& coordinates.y > m_lowerY && coordinates.x < m_upperY) {
				return true;
		}
		return false;
	}

	ModelCoordinate SearchSpace::translateCoordsToSearchSpace(const ModelCoordinate& coords) const {
		ModelCoordinate newcoords;
		newcoords.x = coords.x - m_lowerX;
		newcoords.y = coords.y - m_lowerY;
		return newcoords;
	}

	int SearchSpace::convertCoordToInt(const ModelCoordinate& coord) const {
		ModelCoordinate newcoords = translateCoordsToSearchSpace(coord);
		return newcoords.x + (newcoords.y * getWidth());
	}

	ModelCoordinate SearchSpace::convertIntToCoord(const int cell) const {
		ModelCoordinate coord;
		int width = getWidth();
		coord.x = (cell % width) + m_lowerX;
		coord.y = (cell / width) + m_lowerY;
		return coord;
	}
}
