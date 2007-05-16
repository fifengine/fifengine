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
#include <cmath>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/imagecache.h"
#include "util/rect.h"

#include "layer.h"
#include "geometry.h"
#include "selection.h"

namespace FIFE { namespace map {

	Selection::Selection(LayerPtr layer)
		: m_mode(SingleSelectionMode),
		m_imageid(0),
		m_layer(layer),
		m_selected(false)
	{
		if( layer ) {
			std::string imagefile = layer->get<std::string>("_OVERLAY_IMAGE");
			Point offset = layer->get<Point>("_OVERLAY_IMAGE_OFFSET");
			RenderableLocation location(RenderAble::RT_IMAGE, imagefile);

			if( offset.x ) {
				location.addExtension( RenderableLocation::X, offset.x );
			}

			if( offset.y ) {
				location.addExtension( RenderableLocation::Y, offset.y );
			}

			m_imageid = ImageCache::instance()->addImageFromLocation(location);
		}
	}

	Selection::~Selection() {
	}

	void Selection::setImage(size_t imageid) {
		m_imageid = imageid;
	}

	void Selection::setMode(Selection::Mode mode) {
		m_mode = mode;
	}

	Selection::Mode Selection::getMode() const {
		return m_mode;
	}

	void Selection::select(const Point& p) {

		if( m_mode == SingleSelectionMode ) {
			m_selection = p;
		}

		if( m_mode == MultiSelectionMode && !isSelected(p) ) {
			m_multiselection.push_back( p );
		}

		m_selected = true;
	}

	void Selection::unselect(const Point& p) {
		if( !m_selected )
			return;

		if( m_mode == SingleSelectionMode ) {
			m_selection = p;
			return;
		}

		if( m_mode == MultiSelectionMode ) {
			size_t size = m_multiselection.size();
			for(size_t i=0; i!= size; ++i) {
				if( m_multiselection[i] == p ) {
					m_multiselection[i] = m_multiselection[ size - 1 ];
					m_multiselection.pop_back();
					return;
				}
			}
			return;
		}
	}

	bool Selection::isSelected(const Point& p) const {
		if( !m_selected )
			return false;

		if( m_mode == SingleSelectionMode ) {
			return m_selection == p;
		}

		if( m_mode == MultiSelectionMode ) {
			for(size_t i=0; i!= m_multiselection.size(); ++i) {
				if( m_multiselection[i] == p )
					return true;
			}
			return false;
		}
		return false;
	}

	void Selection::clearSelection() {
		m_selected = false;
		m_multiselection.clear();
	}

	void Selection::render(Screen* screen, const Point& offset) {
		if( !m_selected )
			return;

		Geometry *geometry = m_layer->getGeometry();

		RenderAble* image = ImageCache::instance()->getImage( m_imageid );

		Point pos = geometry->toScreen(m_selection) - offset;
		Rect target(pos.x,pos.y,image->getWidth(),image->getHeight());
		image->render(target,screen);
	}


} } // FIFE::map
