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
#include <string>

// 3rd party library includes
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"
#include "util/purge.h"
#include "util/exception.h"
#include "map/geometries/geometry.h"

#include "elevation.h"
#include "layer.h"

namespace FIFE { namespace map {

	long Elevation::m_count = 0;

	ElevationPtr Elevation::create() {
		ElevationPtr elevation(new Elevation);
		elevation->m_self = elevation;
		return elevation;
	}

	Elevation::Elevation() 
		: AttributedClass("map_elevation"),
		m_reference_layer(0) {
		m_count += 1;
	}

	Elevation::~Elevation() {
		m_count -= 1;
	}

	MapPtr Elevation::getMap() {
		return m_map.lock();
	}

	long Elevation::globalCount() {
		return m_count;
	}

	size_t Elevation::getNumLayers() const {
		return m_layers.size();
	}

	void Elevation::addLayer(LayerPtr layer) {
		if( !layer ) {
			throw NotSupported("can't add empty layer");
		}
		if( layer->getElevation() ) {
			throw Duplicate("layer already contained in an elevation");
		}

		layer->m_layer_num = m_layers.size();
		layer->m_elevation = m_self;
		m_layers.push_back(layer);
	}

	LayerPtr Elevation::getLayer(size_t index) {
		if( index >= getNumLayers() ) {
			throw IndexOverflow(std::string("invalid layer number: ") +
			                    boost::lexical_cast<std::string>(index));
		}
		return m_layers[index];
	}

	void Elevation::insertLayer(size_t index, LayerPtr layer) {
		if( !layer ) {
			throw NotSupported("can't add empty layer");
		}
		if( layer->getElevation() ) {
			throw Duplicate("layer already contained in an elevation");
		}

		if( index >= getNumLayers() ) {
			throw IndexOverflow(std::string("invalid layer number: ") +
			                    boost::lexical_cast<std::string>(index));
		}
		m_layers.insert(m_layers.begin()+index,layer);
		layer->m_elevation = m_self;
		resetLayerNumbers();
	}

	void Elevation::removeLayer(size_t index) {
		if( index >= getNumLayers() ) {
			throw IndexOverflow(std::string("invalid layer number: ") +
			                    boost::lexical_cast<std::string>(index));
		}
		m_layers[index]->m_elevation.reset();
		m_layers.erase(m_layers.begin()+index);
		resetLayerNumbers();
	}

	void Elevation::clearLayers() {
		for(size_t i = 0; i!= getNumLayers(); ++i) {
			m_layers[i]->m_elevation.reset();
		}
		m_layers.clear();
	}

	void Elevation::resetLayerNumbers() {
		for(size_t i=0; i!=m_layers.size(); ++i) {
			m_layers[i]->m_layer_num = i;
		}
	}

	void Elevation::setReferenceLayer(size_t layer) {
		m_reference_layer = layer;
	}

	LayerPtr Elevation::getReferenceLayer() {
		return getLayer(m_reference_layer);
	}

	Point Elevation::centerOfMass() {
		size_t n = 0;
		Point p;

		// Sanity checks.
		LayerPtr ref_layer = getReferenceLayer();
		if( !ref_layer )
			return p;

		Geometry* ref_geo = ref_layer->getGeometry();
		if( ref_geo == 0 )
			return p;


		type_layers::iterator end = m_layers.end();
		for (type_layers::iterator i = m_layers.begin(); i != end; ++i) {
			Point pos;
			Geometry *geo = (*i)->getGeometry();

			if( !(*i)->hasTiles() ) {
				continue;
			}

			for(pos.x=0; pos.x != (*i)->getSize().x; ++pos.x) {
				for(pos.y=0; pos.y != (*i)->getSize().y; ++pos.y) {
					if( (*i)->getTileImage(pos) ) {
						p += ref_geo->fromScreen(geo->toScreen(pos));
						++n;
					}
				}
			}
		}

		if( n > 0 )
			return p/n;
		return  p;
	}

} } //FIFE::map
