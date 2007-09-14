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
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/exception.h"
#include "util/purge.h"
#include "model/metamodel/grids/cellgrid.h"

#include "layer.h"
#include "instance.h"
#include "elevation.h"

namespace FIFE {
	static std::string INVALID_LAYER_SET = "Cannot set layer coordinates, given layer is not initialized properly";
	static std::string INVALID_LAYER_GET = "Cannot get layer coordinates, layer is not initialized properly";
	
	Location::Location() {
		reset();
	}

	Location::Location(const Location& loc) {
		reset();
		m_layer = loc.m_layer;
		m_elevation_coords = loc.m_elevation_coords;
	}
	
	Location::~Location() {
		reset();
	}
	
	void Location::reset() {
		m_elevation_coords.x = 0;
		m_elevation_coords.y = 0;
		m_layer = NULL;
	}
	
	Location& Location::operator=(const Location& rhs) {
		m_layer = rhs.m_layer;
		m_elevation_coords.x = rhs.m_elevation_coords.x;
		m_elevation_coords.y = rhs.m_elevation_coords.y;
		return *this;
	}
	
	bool Location::operator==(const Location& loc) const {
		return ((m_layer == loc.m_layer) && (m_elevation_coords == loc.m_elevation_coords));
	}
		
	bool Location::operator!=(const Location& loc) const {
		return !(*this == loc);
	}
	
	Elevation* Location::getElevation() const {
		if (!m_layer) {
			return NULL;
		}
		return m_layer->getElevation();
	}
		
	void Location::setLayer(Layer* layer) {
		reset();
		m_layer = layer;
	}
	
	Layer* Location::getLayer() const {
		return m_layer;
	}
	
	void Location::setExactLayerCoordinates(const ExactModelCoordinate& coordinates) throw(NotSet) {
		if (!isValid()) {
			throw NotSet(INVALID_LAYER_SET);
		}
		m_elevation_coords = m_layer->getCellGrid()->toElevationCoordinates(coordinates);
	}
	
	void Location::setLayerCoordinates(const ModelCoordinate& coordinates) throw(NotSet) {
		if (!isValid()) {
			throw NotSet(INVALID_LAYER_SET);
		}
		m_elevation_coords = m_layer->getCellGrid()->toElevationCoordinates(coordinates);
	}
	
	void Location::setElevationCoordinates(const ExactModelCoordinate& coordinates) {
		if (!isValid()) {
			throw NotSet(INVALID_LAYER_SET);
		}
		m_elevation_coords = coordinates;
	}
	
	ExactModelCoordinate Location::getExactLayerCoordinates() const throw(NotSet) {
		return getExactLayerCoordinates(m_layer);
	}
	
	ModelCoordinate Location::getLayerCoordinates() const throw(NotSet) {
		return getLayerCoordinates(m_layer);
	}
	
	ExactModelCoordinate Location::getElevationCoordinates() const {
		return m_elevation_coords;
	}
	
	bool Location::isValid() const {
		return isValid(m_layer);
	}
	
	bool Location::isValid(const Layer* layer) const {
		return (layer && layer->getCellGrid());
	}
	
	ExactModelCoordinate Location::getExactLayerCoordinates(const Layer* layer) const throw(NotSet) {
		if (!isValid(layer)) {
			throw NotSet(INVALID_LAYER_GET);
		}
		return layer->getCellGrid()->toExactLayerCoordinates(m_elevation_coords);
	}
	
	ModelCoordinate Location::getLayerCoordinates(const Layer* layer) const throw(NotSet) {
		if (!isValid(layer)) {
			throw NotSet(INVALID_LAYER_GET);
		}
		return layer->getCellGrid()->toLayerCoordinates(m_elevation_coords);
	}
	
	double Location::getCellOffsetDistance() const {
		ExactModelCoordinate pt  = getExactLayerCoordinates();
		double dx = pt.x - static_cast<double>(static_cast<int>(pt.x));
		double dy = pt.y - static_cast<double>(static_cast<int>(pt.y));
		return sqrt(dx*dx + dy*dy);
	}
	
	std::ostream& operator<<(std::ostream& os, const Location& l) {
		ExactModelCoordinate p = l.getExactLayerCoordinates();
		return os << "x=" << p.x << ", y=" << p.y;
	}
}
