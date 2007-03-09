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
#include "util/purge.h"
#include "exception.h"

#include "elevation.h"
#include "layer.h"
#include "geometry.h"

namespace FIFE { namespace map {
	Elevation::Elevation(const elevation_info& structure) 
		: AttributedClass("map_elevation"),
		m_reference_layer(0) {

		setNumLayers(structure.numberOfLayers);
		for (size_t i = 0; i != structure.numberOfLayers; ++i) {
			LayerPtr layer(new Layer(structure.layers[i].size,
			                         structure.layers[i].geometry));
			layer->setShift(structure.layers[i].shift);
			layer->set("_OVERLAY_IMAGE",structure.layers[i].overlay);
			layer->set("_OVERLAY_IMAGE_OFFSET", structure.layers[i].overlay_offset);

			setLayer(i, layer);
		}
		setReferenceLayer(structure.referenceLayer);
	}

	Elevation::~Elevation() {
	}

	size_t Elevation::getNumLayers() const {
		return m_layers.size();
	}

	void Elevation::setNumLayers(size_t n) {
		m_layers.resize(n);
	}

	void Elevation::setLayer(size_t idx, LayerPtr Layer) {
		m_layers[idx] = Layer;
	}
	void Elevation::addLayer(LayerPtr layer) {
		m_layers.push_back(layer);
	}

	LayerPtr Elevation::getLayer(size_t idx) {
		return m_layers.at(idx);
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

		if( n > 0 )
			return p/n;
		return  p;
	}

} } //FIFE::map
