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
#include "video/renderable.h"
#include "view/effects/visualeffect.h"
#include "util/debugutils.h"
#include "video/imagecache.h"
#include "model/geometries/geometry.h"
#include "model/structures/objectinfo.h"
#include "model/structures/layer.h"

#include "visual.h"

namespace FIFE { namespace map { 

	Visual::Visual(ObjectPtr moi) 
		: 
		m_renderable(0),
		m_screenbox() {
		m_alpha = 255;
		m_moi = moi;
		m_zvalue = 0;
		if( moi ) {
			m_zvalue = moi->getZValue();
			m_layer  = moi->getLayer();
		}
	}

	Visual::~Visual() {
		if( m_moi ) {
			m_moi->setVisualId(0);
		}
		// We have to be careful here, as effects remove themselves.
		while( !m_effects.empty() )
			delete *m_effects.begin();
	}

	void Visual::setRenderable(size_t renderable, RenderAble::RenderableTypes type) {
		m_renderable = renderable;
		m_renderableType = type;
	}

	void Visual::calculateScreenbox(RenderAble* r) {
		Point p = m_position;
		m_screenbox.x = r->getXShift() - r->getWidth()/2 + p.x;
		m_screenbox.y = r->getYShift() - r->getHeight()  + p.y;
		m_screenbox.w = r->getWidth();
		m_screenbox.h = r->getHeight();

	}

	void Visual::setLayer(LayerPtr layer) {
		m_layer = layer;
		reset();
	}

	void Visual::reset() {
		if( !m_renderable || !m_layer ) {
			m_screenbox = Rect();
			return;
		}

		if( m_moi ) {
			m_grid_position = m_moi->getPosition();
		}

		Geometry* geometry = m_layer->getGeometry();
		m_position = geometry->toScreen( m_grid_position );
		m_linearposition = m_grid_position.x + m_grid_position.y * m_layer->getSize().x;

		RenderAble* r = ImageCache::instance()->getImage( m_renderable );
		calculateScreenbox(r);

		m_layer_num = m_layer->getLayerNumber();
	}

	void Visual::render(Screen* screen, const Point& shift, uint8_t alpha) {
		Rect target;
		RenderAble* r = ImageCache::instance()->getImage( m_renderable );
		calculateScreenbox(r);

		target = Rect(
			m_screenbox.x - shift.x,
			m_screenbox.y - shift.y,
			m_screenbox.w,
			m_screenbox.h);
		r->render( target, screen, std::min(m_alpha,alpha) );
	}

	void Visual::addEffect(effect::Effect* ve) {
		m_effects.insert(ve);
	}
	void Visual::removeEffect(effect::Effect* ve) {
		m_effects.erase(ve);
	}

} } 
