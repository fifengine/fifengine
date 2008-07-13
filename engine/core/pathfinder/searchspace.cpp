/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
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
#include "model/structures/map.h"

#include "searchspace.h"

namespace FIFE {

	SearchSpace::SearchSpace(Layer* layer) 
	: m_upperX(0), m_upperY(0), m_lowerX(0), m_lowerY(0), m_layer(layer) {
		
		Map* map = layer->getMap();
		const std::list<Layer*>& layers = map->getLayers();
		ModelCoordinate min, max;

		for(std::list<Layer*>::const_iterator i = layers.begin();
			i != layers.end();
			++i) {

			ModelCoordinate newMin, newMax;
			(*i)->getMinMaxCoordinates(newMin, newMax, layer);

			if(newMin.x < min.x) {
				min.x = newMin.x;
			} 
			
			if(newMax.x > max.x) {
				max.x = newMax.x;
			}

			if(newMin.y < min.y) {
				min.y = newMin.y;
			}
			
			if(newMax.y > max.y) {
				max.y = newMax.y;
			}
		}

		m_upperX = max.x;
		m_upperY = max.y;
		m_lowerX = min.x;
		m_lowerY = min.y;
	}

	bool SearchSpace::isInSearchSpace(const Location& location) const {
		if(location.getLayer() != m_layer) {
			return false;
		}
		ModelCoordinate coordinates = location.getLayerCoordinates();
		if(coordinates.x >= m_lowerX && coordinates.x <= m_upperX 
			&& coordinates.y >= m_lowerY && coordinates.y <= m_upperY) {
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

	int SearchSpace::getMaxIndex() const {

		//max index = w * h.
		int max_index = getWidth() + (getWidth() * getHeight());
		return max_index;
	}
}
