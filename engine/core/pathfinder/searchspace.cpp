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
#include <limits>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/instance.h"
#include "model/structures/layer.h"

#include "searchspace.h"

namespace FIFE {

	SearchSpace::SearchSpace(Layer* layer) 
	: m_layer(layer), m_lowerX(0), m_lowerY(0), m_upperX(0), m_upperY(0) {
		//Loop through all instances, should we really be doing this in the constructor?
		const std::vector<Instance*>& instances = m_layer->getInstances();
		//Set all search space limits to + and - infinity respectively.
		m_lowerX = std::numeric_limits<int>::infinity();
		m_lowerY = std::numeric_limits<int>::infinity();
		m_upperX = -std::numeric_limits<int>::infinity();
		m_upperY = -std::numeric_limits<int>::infinity();
		//Iterate through all instances and formulate the search space.
		for(std::vector<Instance*>::const_iterator i = instances.begin();
			i != instances.end();
			++i) 
		{
			Location loc = (*i)->getLocation();
			ModelCoordinate coord = loc.getLayerCoordinates();
			if(coord.x < m_lowerX) {
				m_lowerX = coord.x;
			} else if(coord.x > m_upperX) {
				m_upperX = coord.x;
			}

			if(coord.y < m_lowerY) {
				m_lowerY = coord.y;
			} else if(coord.y > m_upperY) {
				m_upperY = coord.y;
			}
		}
	}

	bool SearchSpace::isInSearchSpace(const Location& location) const {
		if(location.getLayer() != m_layer) {
			return false;
		}
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
