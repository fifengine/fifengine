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
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "elevation.h"
#include "layer.h"
#include "object_iterator.h"

namespace FIFE { namespace map {

	ObjectIterator::ObjectIterator( ElevationPtr elevation) : m_elevation(elevation) {
		// SKIP TO A LAYER THAT HAS OBJECTS.
		m_layer = 0;
		m_object = 0;
		if( !m_elevation ) {
			return;
		}
		while( m_layer < m_elevation->getNumLayers()
		       && !m_elevation->getLayer(m_layer)->hasObjects() ) {
			++m_layer;
		}
		if( m_layer == m_elevation->getNumLayers() ) {
			m_elevation.reset();
			return;
		}
	}

	ObjectPtr ObjectIterator::next() {
		if( !m_elevation ) {
			return ObjectPtr();
		}
		// We have a valid layer. Find a gridpos with objects.
		LayerPtr layer = m_elevation->getLayer(m_layer);
		ObjectList& objects = layer->getObjectsAt( m_gridpos );
		while( objects.size() == m_object ) {
			m_object = 0;
			m_gridpos.x += 1;
			if( m_gridpos.x == layer->getSize().x ) {
				m_gridpos.y += 1;
				m_gridpos.x  = 0;
				if( m_gridpos.y == layer->getSize().y ) {
					break;
				}
			}
			objects = layer->getObjectsAt( m_gridpos );
		}

		if( m_gridpos.y == layer->getSize().y ) {
			// Reached end-of-layer
			// Advance and recurse, once we find a valid one again.
			++m_layer;
			m_gridpos = Point();
			while( m_layer < m_elevation->getNumLayers()
				&& !m_elevation->getLayer(m_layer)->hasObjects() ) {
				++m_layer;
			}
			if( m_layer == m_elevation->getNumLayers() ) {
				m_elevation.reset();
				return ObjectPtr();
			}
			return next();
		}
		return objects.at( m_object++ );
	}

}}
