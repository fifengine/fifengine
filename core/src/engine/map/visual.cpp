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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "video/complexanimation.h"
#include "video/renderable.h"
#include "map/effects/visualeffect.h"
#include "imagecache.h"
#include "debugutils.h"

#include "geometry.h"
#include "objectinfo.h"
#include "visual.h"

namespace FIFE { namespace map { 

	Visual::Visual(ObjectInfo* moi) 
		: 
		m_renderable(0),
		m_screenbox() {
		m_alpha = 255;
		m_moi = moi;
		m_renderableCopy = 0;
		m_zvalue = 0;
		if( moi ) {
			m_zvalue = moi->getZValue();
		}
	}

	Visual::~Visual() {
		if( m_moi ) {
			m_moi->setVisualId(0);
		}
		delete m_renderableCopy;
		// We have to be careful here, as effects remove themselves.
		while( !m_effects.empty() )
			delete *m_effects.begin();
	}

	void Visual::setRenderable(size_t renderable, RenderAble::RenderableTypes type) {
		m_renderable = renderable;
		m_renderableType = type;

		if( m_renderableCopy ) {
			delete m_renderableCopy;
			m_renderableCopy = 0;
		}

		if( type == RenderAble::RT_COMPLEX_ANIMATION ) {
			RenderAble* r = ImageCache::instance()->getImage( renderable );
			if( r->getType() !=  RenderAble::RT_COMPLEX_ANIMATION ) {
				m_renderableType = r->getType();
				return;
			}
			m_renderableCopy = dynamic_cast<ComplexAnimation*>(r);
			m_renderableCopy = new ComplexAnimation(*m_renderableCopy);
		}
	}

	void Visual::calculateScreenbox(RenderAble* r) {
		Point p = m_position;
		m_screenbox.x = r->getXShift() - r->getWidth()/2 + p.x;
		m_screenbox.y = r->getYShift() - r->getHeight()  + p.y;
		m_screenbox.w = r->getWidth();
		m_screenbox.h = r->getHeight();

	}

	void Visual::reset(Grid* grid) {

		if( !m_renderable ) {
			m_screenbox = Rect();
			return;
		}

		Geometry* geometry = grid->getGeometry();
		m_position = geometry->toScreen( m_location.position );
		m_linearposition = m_location.position.x + m_location.position.y * grid->getSize().x;

		if (m_renderableType == RenderAble::RT_COMPLEX_ANIMATION) {
			ComplexAnimation* anim = dynamic_cast<ComplexAnimation*>(m_renderableCopy);
			anim->resetGeometry( geometry );
			calculateScreenbox(anim);
		} else { 
			RenderAble* r = ImageCache::instance()->getImage( m_renderable );
			calculateScreenbox(r);
		}
	}

	void Visual::render(Screen* screen, const Point& shift, uint8_t alpha) {
		Rect target;
		RenderAble *r = m_renderableCopy;

		if( !r ) {
			r = ImageCache::instance()->getImage( m_renderable );
		}// else {
		calculateScreenbox(r);
		//}

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
