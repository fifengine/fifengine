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

#include "elevation.h"
#include "map.h"
#include "layer.h"

namespace FIFE { namespace model {

	Elevation::Elevation() 
		: AttributedClass("map_elevation"),
		m_reference_layer(0) {
	}

	Elevation::~Elevation() {

	}

	Map* Elevation::getMap() {
		return m_map;	
	}

	size_t Elevation::getNumLayers() const {
		return m_layers.size();
	}

	Layer* Elevation::addLayer(GeometryType* geometry) {
		Layer* layer = new Layer(geometry);
		layer->m_elevation = this;
		m_layers.push_back(layer);
		return layer;
	}

	void Elevation::removeLayer(Layer* layer) {
		std::vector<Layer*>::iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			if((*it) == layer) {
				delete *it;
				m_layers.erase(it);
				return ;
			}
		}
	}

	void Elevation::clearLayers() {
		purge(m_layers);
		m_layers.clear();
	}

	void Elevation::setReferenceLayer(Layer* layer) {
		m_reference_layer = layer;
	}

	Layer* Elevation::getReferenceLayer() {
		return m_reference_layer;
	}

	Point Elevation::centerOfMass() {
		// TODO: this logic needs to be reconsidered now that there
		// isn't a distinction between tiles and objects
		
		return Point(0,0);
/*		size_t n = 0;
		Point p;

		// Sanity checks.
		Layer* ref_layer = getReferenceLayer();
		if( !ref_layer )
			return p;

		Geometry* ref_geo = ref_layer->getGeometry();
		if( ref_geo == 0 )
			return p;


		std::vector<Layer*>::iterator end = m_layers.end();
		for (std::vector<Layer*>::iterator i = m_layers.begin(); i != end; ++i) {
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
		return  p; */
	}

} } //FIFE::model
