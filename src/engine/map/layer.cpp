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
#include "geometry.h"
#include "layer.h"

namespace FIFE { namespace map {

	Layer::Layer(const Point& size, size_t geometry)
		: AttributedClass("map_Layer"),
		m_size(size),
		m_global_alpha(255),
 		m_shift(),
		m_geometry(Geometry::createGeometry(geometry,size)) {

		m_tiles_visibility = m_objects_visibility = true;
		m_grid_overlay = false;

		// set default attributes
		set<std::string>("_OVERLAY_IMAGE","content/gfx/tiles/tile_outline.png");
	}

	Layer::~Layer() {
		delete m_geometry;
	}

	const Point& Layer::getSize() const {
		return m_size;
	}

	Geometry* Layer::getGeometry() {
		return m_geometry;
	}

	void Layer::setShift(const Point& shift) {
		m_shift = shift;
	}

	const Point& Layer::getShift() const {
		return m_shift;
	}
	
	bool Layer::hasObjects() const {
		return !m_objects.empty();
	}

	void Layer::addObject(ObjectPtr object) {
		addObjectAt( object, object->getLocation().position );
	}
	void Layer::addObjectAt(ObjectPtr object, int32_t x,int32_t y) {
		if( !hasObjects() ) {
			m_objects.resize(m_size.y * m_size.x);
		}
		m_objects.at(x + y * m_size.x).append( object );
	}

	ObjectList& Layer::getObjectsAt(int32_t x,int32_t y) {
		if( hasObjects() ) {
			m_objects.resize(m_size.y * m_size.x);
		}
		return m_objects.at(x + y * m_size.x);
	}

	const ObjectList& Layer::getObjectsAt(int32_t x,int32_t y) const {
		static ObjectList objects;
		if( hasObjects() ) {
			return m_objects.at(x + y * m_size.x);
		}
		return objects;
	}

	void Layer::addObjectAt(ObjectPtr object, const Point& p) {
		if( !hasObjects() ) {
			m_objects.resize(m_size.y * m_size.x);
		}
		m_objects.at(p.x + p.y * m_size.x).append( object );
	}

	ObjectList& Layer::getObjectsAt(const Point& p) {
		if( hasObjects() ) {
			m_objects.resize(m_size.y * m_size.x);
		}
		return m_objects.at(p.x + p.y * m_size.x);
	}

	const ObjectList& Layer::getObjectsAt(const Point& p) const {
		static ObjectList objects;
		if( hasObjects() ) {
			return m_objects.at(p.x + p.y * m_size.x);
		}
		return objects;
	}

	void Layer::setTileImage(int32_t x, int32_t y, size_t id) {
		setTileImage(Point(x,y),id);
	}
	void Layer::setTileImage(const Point& p, size_t id) {
		if (m_tiles.empty()) {
			m_tiles.resize(m_size.x * m_size.y);
		}
		if (!isValidPosition(p)) {
			return;
		}
		m_tiles[p.x + p.y * m_size.x] = id;
	}

	bool Layer::hasTiles() const {
		return !m_tiles.empty();
	}

	void Layer::setTilesVisible(bool vis) {
		m_tiles_visibility = vis;
	}
	void Layer::toggleTilesVisible() {
		m_tiles_visibility = !m_tiles_visibility;
	}
	bool Layer::areTilesVisible() const {
		return m_tiles_visibility;
	}

	void Layer::setObjectsVisible(bool vis) {
		m_objects_visibility = vis;
	}
	void Layer::toggleObjectsVisible() {
		m_objects_visibility = !m_objects_visibility;
	}
	bool Layer::areObjectsVisible() const {
		return m_objects_visibility;
	}

	uint8_t Layer::getGlobalAlpha() const {
		return m_global_alpha;
	}

	void Layer::setGlobalAlpha(uint8_t alpha) {
		m_global_alpha = alpha;
	}

	void Layer::addToGlobalAlpha(int delta) {
		m_global_alpha = std::min(255, std::max(0, m_global_alpha + delta));
	}

	bool Layer::isGridOverlayEnabled() const {
		return m_grid_overlay;
	}
	void Layer::setGridOverlayEnabled(bool e) {
		m_grid_overlay = e;
	}
	void Layer::toggleGridOverlayEnabled() {
		m_grid_overlay = !m_grid_overlay;
	}

	struct Layer_clear_visitor : public boost::static_visitor<> {
		void operator()(std::vector<long>& v)  const { v.clear(); }
		void operator()(std::vector<bool>& v)  const { v.clear(); }
		void operator()(std::vector<float>& v) const { v.clear(); }
	};

	void Layer::removeParam(uint8_t param_id) {
		boost::apply_visitor( Layer_clear_visitor(), m_paramLayers.at(param_id) );
		m_param_freelist.push_back( param_id );

		type_paramnames_reverse::iterator it;
		it = m_paramnames_reverse.find( param_id );

		m_paramnames.erase( it->second );
		m_paramnames_reverse.erase( it );
	}

} } // FIFE::map
