/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#include "mapview.h"
#include "map.h"
#include "mapelevation.h"
#include "object.h"
#include "debugutils.h"
#include "imagecache.h"
#include "animation.h"
#include "log.h"
#include "video/screen.h"
#include "video/renderable.h"
#include "exception.h"

#include <SDL.h>
#include <algorithm>
#include <cmath>
#include "mapgeometry.h"

#include "defaulttilegeometry.h"
#include "defaultobjectgeometry.h"

#include "maptilegrid.h"

namespace FIFE {

	/// Included here until geometries are complete. (#54)
	static const unsigned int TILE_WIDTH = 80;
	static const unsigned int TILE_HEIGHT = 36;
	
	// if we move 1 to the right we need to add this to the last xpos
	static const int X_DELTA_X = 48; 
	// if we move 1 to the right we need to add this to the last ypos
	static const int X_DELTA_Y = 12; 
	// if we move 1 tile down we need to add this to the last xpos
	static const int Y_DELTA_X = 32; 
	// if we move 1 tile down we need to add this to the last ypos
	static const int Y_DELTA_Y = 24; 


	MapView::MapView() 
		: m_surface(0),
		m_rect(),
		m_map(0),
		m_elevation(0),
		m_tile_geometry( new DefaultTileGeometry() ),
		m_obj_geometry( new DefaultObjectGeometry() ),
		m_xpos(0), m_ypos(0),
		m_x_range(0), m_y_range(0),
		m_elevation_id(0),
		m_render_floor(true), m_render_objects(true), m_render_roof(true),
		m_render_tile_grid(false), m_render_obj_grid(false),
		m_grid_pos(-1) {
		// Quick hack for getting the masks.
		// msef003.frm >> hex outline
		// msef000.frm >> hex outline
		m_tilemask = ImageCache::instance()->addImageFromFile("content/gfx/tiles/tile_outline.png");
		//m_objmask =  ImageCache::instance()->addImageFromFile("tilemask_grey.bmp");

	}

	MapView::~MapView() {}

	MapView::MapView(const MapView&) 
		: m_surface(0),
		m_rect(),
		m_map(0),
		m_elevation(0),
		m_tile_geometry( new DefaultTileGeometry() ),
		m_obj_geometry( new DefaultObjectGeometry() ),
		m_xpos(0), m_ypos(0),
		m_x_range(0), m_y_range(0),
		m_elevation_id(0),
		m_render_floor(true), m_render_objects(true), m_render_roof(true),
		m_grid_pos() {
	}

	MapView& MapView::operator=(const MapView&) {
		return *this;
	}

	void MapView::setViewport(Screen* surface) {
		if( !surface ) {
			// Here I assume that the call setViewport(0) in MapViewGameState::deactivate()
			// was made on purpose! -> Inhibit all calls in render()
			m_surface = 0;
			return;
		}
		setViewport(surface, Rect(0, 0, surface->getWidth(), surface->getHeight()));
		//updateOffsets();
	}

	void MapView::setViewport(Screen* surface, const Rect& rect) {
	/*
		double phi_a, phi_b, phi_c, phi_d, phi_e, phi_f;
		double X1, X2, Y1, Y2;
	*/
		m_surface = surface;
		m_rect = rect;
	/*
		phi_a = atan(X_DELTA_Y/double(X_DELTA_X));
		phi_b = M_PI / 2 - phi_a;
		phi_c = atan(Y_DELTA_Y/double(Y_DELTA_X));
		phi_d = M_PI / 2 - phi_c;
		phi_e = M_PI - phi_b - phi_d;
		phi_f = M_PI - phi_a - phi_c;

		X1 = m_rect.h / sin(phi_e) * sin(phi_d);
		X2 = m_rect.w / sin(phi_f) * sin(phi_c);
		m_x_range = int(ceil((X1 + X2)/double(sqrt(X_DELTA_X*X_DELTA_X + X_DELTA_Y*X_DELTA_Y)))) + 1;

		Y1 = m_rect.h / sin(phi_e)*sin(phi_b);
		Y2 = m_rect.w / sin(phi_f)*sin(phi_c);
		m_y_range = int(ceil((Y1 + Y2)/double(sqrt(Y_DELTA_X*Y_DELTA_X + Y_DELTA_Y*Y_DELTA_Y)))) + 1;
	*/
	}

	void MapView::setMap(Map* map, size_t elevation) {
		m_elevation_id = elevation;
		m_map = map;
		m_elevation = m_map->getElevation(elevation);
	}

	void MapView::setXPos(unsigned int pos) {
		m_offset.x = pos;
	}

	void MapView::setYPos(unsigned int pos) {
		m_offset.y = pos;
	}

	int MapView::getXPos() const {
		return m_offset.x;
	}

	int MapView::getYPos() const {
		return m_offset.y;
	}

	void MapView::setRoofVisible(bool v) {
		m_render_roof = v;
	}

	void MapView::applyRoofAlpha(int alpha) {
		m_elevation->getTileGrid(1)->addToGlobalAlpha(alpha);
	}

	void MapView::setRoofAlpha(int alpha) {
		alpha = std::min( 255, std::max( 0, alpha ) );
		m_elevation->getTileGrid(1)->setGlobalAlpha(alpha);
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

	void MapView::setTileGridVisible(bool v) {
		m_render_tile_grid = v;
	}

	bool MapView::getTileGridVisible() const {
		return m_render_tile_grid;
	}

	void MapView::setObjectGridVisible(bool v) {
		m_render_obj_grid = v;
	}

	bool MapView::getObjectGridVisible() const {
		return m_render_obj_grid;
	}

	bool MapView::getObjectsVisible() const {
		return m_render_objects;
	}

	bool MapView::getFloorVisible() const {
		return m_render_floor;
	}




	void MapView::renderObjects() {
		// 		RenderContext rc = MapGeometry::createContext(m_rect, m_xpos, m_ypos);
		const MapElevation::type_objects& objects = m_elevation->getObjects();
		MapElevation::type_objects::const_iterator end = objects.end();

		/*
		int x_off = 0 - m_surface->getWidth() - m_xpos * X_DELTA_X - m_ypos * Y_DELTA_X;
		int y_off = 0 - m_ypos * Y_DELTA_Y + m_xpos * X_DELTA_Y - 96;
		*/

		/* this is a quick fix to maintain correct drawing order of objects; I don't
		 * think it is needed for now (xmlmap does!) and there should be a less expensive
		 * way of doing it anyway...
		 */

		/*
		typedef std::multimap<int, MapElevation::type_objects::const_iterator> TmpMapType;
		typedef TmpMapType::value_type TmpValueType;
		TmpMapType objectsInView;
		for (MapElevation::type_objects::const_iterator i = objects.begin(); i != end; ++i) {
			Object * obj = *i;
			int pos = obj->getPosition();
			if (pos == -1)
				return;
			objectsInView.insert(TmpValueType(pos, i));
		}
		TmpMapType::const_iterator end2 = objectsInView.end();
		for (TmpMapType::const_iterator i = objectsInView.begin(); i != end2; ++i) {
		*/

		for (MapElevation::type_objects::const_iterator i = objects.begin(); i != end; ++i) {
			Object* obj = *i;
			//Object* obj = *i->second; // activate this for above change, too!
			size_t imgid = obj->getVisual();
			if( !imgid )
				continue;
			RenderAble* img = ImageCache::instance()->getImage(imgid);

			//obj->calcPos();
			int32_t p = obj->getPosition();
			// obj->getRenderPos();
			Point pos = m_obj_geometry->toScreen(Point( p % 200, p / 200 )) - m_offset;

			pos.x -= img->getWidth() / 2  - img->getXShift();
			pos.y -= img->getHeight() - img->getYShift();

			Rect target;
			target.x = pos.x;
			target.y = pos.y;
			target.w = img->getWidth();
			target.h = img->getHeight();


			// 			pos.x += img->getXShift();
			// 			pos.y += img->getYShift();

			img->render(target, m_surface, obj->m_opacity);
		}
	}

	void MapView::loadOverlayImage(const std::string & filename) {
		m_grid_id = ImageCache::instance()->addImageFromFile(filename);
		//m_grid_pos = -1;
	}

	void MapView::renderGrid(MapGrid* grid ) {
		Point offset = m_offset + grid->getShift();
		GridGeometry* geometry = grid->getGeometry();

		int startx, starty, stopx, stopy;
		Rect draw = geometry->gridBoundingRect(Rect(offset.x,offset.y,m_rect.w,m_rect.h));
		startx = draw.x;
		starty = draw.y;
		int x_range = draw.w;
		int y_range = draw.h;
		Point size = grid->getSize();
		stopx = startx + x_range + 2;
		stopx = std::min(size.x, stopx);
		startx = std::max(0, startx);
		stopy = starty + y_range + 2;
		starty = std::max(0,starty);
		stopy = std::min(size.y,stopy);

		Point basesize = geometry->baseSize();
		size_t iid = ImageCache::instance()->addImageFromFile( grid->getGridOverlay() );
		RenderAble* renderable = ImageCache::instance()->getImage( iid );

		Point index;
		for (index.y = starty; index.y < stopy; ++index.y) {
			for (index.x = startx; index.x < stopx; ++index.x) {
				Point pos = geometry->toScreen(index) - offset;
				Rect target(pos.x,pos.y,basesize.x,basesize.y);
				renderable->render(target,m_surface);
			}
		}
	}


	void MapView::renderTiles( MapTileGrid* grid ) {
		Point offset = m_offset + grid->getShift();
		GridGeometry* geometry = grid->getGeometry();

		int startx, starty, stopx, stopy;
		Rect draw = geometry->gridBoundingRect(Rect(offset.x,offset.y,m_rect.w,m_rect.h));
		startx = draw.x;
		starty = draw.y;
		int x_range = draw.w;
		int y_range = draw.h;
		Point size = grid->getSize();
		stopx = startx + x_range + 2;
		stopx = std::min(size.x, stopx);
		startx = std::max(0, startx);
		stopy = starty + y_range + 2;
		starty = std::max(0,starty);
		stopy = std::min(size.y,stopy);

		Point basesize = geometry->baseSize();
		uint8_t alpha = grid->getGlobalAlpha();
		Point index;
		for (index.y = starty; index.y < stopy; ++index.y) {
			for (index.x = startx; index.x < stopx; ++index.x) {
				Point pos = geometry->toScreen(index) - offset;
				Rect target(pos.x,pos.y,basesize.x,basesize.y);
				size_t id = grid->getImageId(index);
				RenderAble* renderable = ImageCache::instance()->getImage(id);
				renderable->render(target,m_surface,alpha);
			}
		}

	}

	void MapView::renderObjectSelection() {
		RenderAble* img = ImageCache::instance()->getImage(m_grid_id);
		// I didn't make the grid an Object, because it really isn't. *g*
		// Most of this functionality should be elsewhere; and support switching
		// between grid-graphics for game-choices (combat, move, pickup)
		Point screenpos = m_obj_geometry->toScreen( m_grid_pos ) - m_offset;
		screenpos.x -= img->getWidth() / 2  - img->getXShift();
		screenpos.y -= img->getHeight() - img->getYShift();
		img->render(Rect(screenpos.x,screenpos.y, img->getWidth(), img->getHeight()), m_surface);
	}

	void MapView::renderTileSelection() {
		RenderAble* image = ImageCache::instance()->getImage(m_tilemask);
		Point basesize = m_tile_geometry->baseSize();
		Point spos = m_tile_geometry->toScreen(m_tilemask_pos) - m_offset;
		Rect target(spos.x,spos.y,basesize.x,basesize.y);
		image->render(target,m_surface);
	}

	void MapView::render() {

		if (!m_map || ! m_surface)
			return;

		if( m_render_floor ) {
			renderTiles(m_elevation->getTileGrid(0));
		}

		if( m_render_tile_grid ) {
			renderGrid(m_elevation->getTileGrid(0));
		}

		if( m_render_obj_grid ) {
			renderGrid(m_elevation->getObjectGrid(0));
		}

		renderObjectSelection();
		renderTileSelection();

		if (m_render_objects) {
			renderObjects();
		}

		if (m_render_roof) {
			renderTiles(m_elevation->getTileGrid(1));
		}

	}

	size_t MapView::getElevationNum() const {
		return m_elevation_id;
	}
	size_t MapView::getElevationCount() const {
		assert(m_map);
		return m_map->getElevationCount();
	}

	void MapView::setElevationNum(size_t id) {
		m_elevation = m_map->getElevation(id);
		m_elevation_id = id;
	}

	MapElevation* MapView::getCurrentElevation() const {
		return m_elevation;
	}


	void MapView::onMouseClick(unsigned char type, unsigned char button, unsigned short x, unsigned short y) {
		Log("mapview") << "Click: type/button x,y " << int(type) << "/" << int(button) << " " << x << "," << y;
		if( button == 3 ) {
			m_tilemask_pos = m_tile_geometry->fromScreen(Point(x,y) + m_offset);
			Log("mapview") << "Selected tile grid: " << m_tilemask_pos;
		} else {
			m_grid_pos = m_obj_geometry->fromScreen(Point(x,y) + m_offset );
			Log("mapview") << "Selected object grid: " << m_grid_pos;
		}
	}


}//FIFE
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
