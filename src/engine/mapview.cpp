/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#include "mapview.h"
#include "map.h"
#include "mapelevation.h"
#include "object.h"
#include "debugutils.h"
#include "imagecache.h"
#include "animation.h"
#include  "log.h"

#include <SDL.h>
#include <algorithm>
#include <math.h>
#include "visual.h"
#include "mapgeometry.h"

namespace FIFE {

	MapView::MapView() :	m_surface(0), m_rect(), m_map(0), m_elevation(0), m_xpos(0), m_ypos(0), m_x_range(0), m_y_range(0),
	m_elevation_id(0), m_render_floor(true), m_render_objects(true), m_render_roof(true), m_render_roof_transparent(false) {}

	MapView::~MapView() {}

	MapView::MapView(const MapView&) : m_surface(0), m_rect(), m_map(0), m_elevation(0), m_xpos(0), m_ypos(0), m_x_range(0), m_y_range(0),
	m_elevation_id(0),  m_render_floor(true), m_render_objects(true), m_render_roof(true), m_render_roof_transparent(false) {}

	MapView& MapView::operator=(const MapView&) {
		return *this;
	}

	void MapView::setViewport(SDL_Surface* surface) {
		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.w = surface->w;
		rect.h = surface->h;
		setViewport(surface, rect);
	}

	void MapView::setViewport(SDL_Surface* surface, const SDL_Rect& rect) {
		double phi_a, phi_b, phi_c, phi_d, phi_e, phi_f;
		double X1, X2, Y1, Y2;
		static const float PI = 3.14159265;

		m_surface = surface;
		m_rect = rect;

		phi_a = atan(X_DELTA_Y/double(X_DELTA_X));
		phi_b = PI/2 - phi_a;
		phi_c = atan(Y_DELTA_Y/double(Y_DELTA_X));
		phi_d = PI/2 - phi_c;
		phi_e = PI - phi_b - phi_d;
		phi_f = PI - phi_a - phi_c;

		X1 = m_rect.h/sin(phi_e)*sin(phi_d);
		X2 = m_rect.w/sin(phi_f)*sin(phi_c);
		m_x_range = int(ceil((X1 + X2)/double(sqrt(X_DELTA_X*X_DELTA_X + X_DELTA_Y*X_DELTA_Y)))) + 1;

		Y1 = m_rect.h/sin(phi_e)*sin(phi_b);
		Y2 = m_rect.w/sin(phi_f)*sin(phi_c);
		m_y_range = int(ceil((Y1 + Y2)/double(sqrt(Y_DELTA_X*Y_DELTA_X + Y_DELTA_Y*Y_DELTA_Y)))) + 1;

	}

	void MapView::setMap(Map* map, size_t elevation) {
		m_elevation_id = elevation;
		m_map = map;
		m_elevation = m_map->getElevation(elevation);
	}

	void MapView::setXPos(unsigned int pos) {
		m_xpos = pos;
	}

	void MapView::setYPos(unsigned int pos) {
		m_ypos = pos;
	}

	int MapView::getXPos() const {
		return m_xpos;
	}

	int MapView::getYPos() const {
		return m_ypos;
	}

	void MapView::setRoofVisible(bool v) {
		m_render_roof = v;
	}

	void MapView::setRoofTransparent(bool v) {
		m_render_roof_transparent = v;
	}

	void MapView::setRoofAlpha(int v) {
		m_roof_alpha = std::min(255,std::max(0,v));
	}

	void MapView::setObjectsVisible(bool v) {
		m_render_objects = v;
	}

	void MapView::setFloorVisible(bool v) {
		m_render_floor = v;
	}

	bool MapView::getRoofVisible() const {
		return m_render_roof;
	}

	bool MapView::getRoofTransparent() const {
		return m_render_roof_transparent;
	}

	int MapView::getRoofAlpha() const {
		return m_roof_alpha;
	}

	bool MapView::getObjectsVisible() const {
		return m_render_objects;
	}

	bool MapView::getFloorVisible() const {
		return m_render_floor;
	}

	void MapView::renderFloor() {
		SDL_Rect target;
		int startx, stopx, starty, stopy;

		startx = m_xpos;
		stopx = startx + m_x_range;
		stopx = std::min(m_elevation->getWidth(),stopx);
		startx = std::max(0,startx);
		starty = m_ypos;
		stopy = starty + m_y_range;
		starty = std::max(0,starty);
		stopy = std::min(m_elevation->getHeight(),stopy);

		RenderContext rc = MapGeometry::createContext(m_rect,m_xpos,m_ypos);

		for (int y = starty; y < stopy; ++y) {
			for (int x = startx; x < stopx; ++x) {
				target = rc.createTileTarget(x, y);

				FIFE::MapTile* tile = m_elevation->getTile(x, y);
				tile->renderFloor(&target, m_surface);
			}
		}
	}

	void MapView::renderRoof() {
		SDL_Rect target;
		int startx, stopx, starty, stopy;

		startx = m_xpos-2;
		stopx = startx + m_x_range;
		stopx = std::min(m_elevation->getWidth(),stopx);
		startx = std::max(0,startx);
		starty = m_ypos-2;
		stopy = starty + m_y_range;
		starty = std::max(0,starty);
		stopy = std::min(m_elevation->getHeight(),stopy);
		RenderContext rc = MapGeometry::createContext(m_rect,m_xpos,m_ypos);
		rc.shift( 0, -96 );
		for (int y = starty; y < stopy; ++y) {
			for (int x = startx; x < stopx; ++x) {
				target = rc.createTileTarget(x,y);

				FIFE::MapTile* tile = m_elevation->getTile(x, y);
				if (m_render_roof_transparent)
					tile->renderRoof(&target, m_surface, m_roof_alpha);
				else
					tile->renderRoof(&target, m_surface);
			}
		}
	}

	void MapView::renderObjects() {
		RenderContext rc = MapGeometry::createContext(m_rect,m_xpos,m_ypos);
		const MapElevation::type_objects& objects = m_elevation->getObjects();
		MapElevation::type_objects::const_iterator end = objects.end();

		for (MapElevation::type_objects::const_iterator i = objects.begin(); i != end; ++i) {
			Object* obj = *i;
			Visual* visual = obj->getVisual();
			visual->render(rc,m_surface);
		}
	}

	void MapView::render() {

		if (!m_map || ! m_surface)
			return;

		if (m_render_floor) {
			renderFloor();
		}

		if (m_render_objects) {
			renderObjects();
		}

		if (m_render_roof) {
			renderRoof();
		}

	}

	size_t MapView::getElevationNum() const {
		return m_elevation_id;
	}
	size_t MapView::getElevationCount() const {
		assert(m_map != NULL);
		return m_map->getElevationCount();
	}

	void MapView::setElevationNum(size_t id) {
		m_elevation = m_map->getElevation(id);
		m_elevation_id = id;
	}

	MapElevation* MapView::getCurrentElevation() const {
		return m_elevation;
	}

}//FIFE
