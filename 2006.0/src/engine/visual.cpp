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
#include "visual.h"
#include "imagecache.h"
#include "animation.h"
#include "graphix_primitives.h"

#include <SDL_video.h>

namespace FIFE {

	struct VisualInternals {
		enum Visual::visualType m_visual_type;
		enum Visual::positionType m_position_type;
		enum Visual::layerType m_layer_type;
		int m_position;
		
		union {
			Animation* m_anim;
			ComplexAnimation* m_complex_anim;
			Image* m_surface;
		};
		int m_xshift;
		int m_yshift;
	};

	Visual::Visual() {
		init();
	}

	Visual::~Visual() {
		cleanup();
	}
	Visual::Visual(const Visual&) {
		init();
	}

	void Visual::init() {
		_d = new VisualInternals();

		setPositionType( PT_HEX_GRID );
		setVisualType( VT_STATIC_IMAGE );
		setLayerType( LT_OBJECT );
		setPosition( POS_NOT_ON_MAP );
	};


	Visual& Visual::operator=(const Visual&) {
		return *this;
	}

	void Visual::cleanup() {
		switch( _d->m_visual_type ) {
		case VT_STATIC_ANIM: 
			delete _d->m_anim;
			break;
		};

		delete _d;
	}

	void Visual::setPosition(int pos) {
		_d->m_position = pos;
	}

	void Visual::setShift(int xshift, int yshift) {
		_d->m_xshift = xshift;
		_d->m_yshift = yshift;
	};

	void Visual::setImage(size_t imgid, int xshift, int yshift) {
		_d->m_surface = ImageCache::instance()->getImage(imgid);
		_d->m_xshift = xshift;
		_d->m_yshift = yshift;
 		setVisualType( VT_STATIC_IMAGE );
	};

	void Visual::setAnimation( Animation* anim ) {
		_d->m_anim = anim;
		setVisualType( VT_STATIC_ANIM );
	};

	int Visual::getPosition() const {
		return _d->m_position;
	};

	void Visual::render(RenderContext rc,SDL_Surface* m_surface) {
		SDL_Rect target;
		int pos = getPosition();
		if( getPositionType() == PT_HEX_GRID ) {
			rc.shift(0,-96);
			target = rc.createObjectTarget(pos % 200, pos / 200);
		} else {
			target = rc.createTileTarget(pos % 100, pos / 100);
		}
		if (getVisualType() == VT_STATIC_IMAGE) {
			Image* image = _d->m_surface;
			if (!image)
				return;
			SDL_Surface *surface = image->getSurface();
			if (!surface)
				return;
			
			target.x -= surface->w/2 - _d->m_xshift;
			target.y -= surface->h - _d->m_yshift;
			
			image->render(&target,m_surface);
			
		} else if (getVisualType() == VT_STATIC_ANIM) {
			int ticks = SDL_GetTicks(); // FIXME: The old update HACK

			size_t ic_idx = _d->m_anim->getCurrentFrame();
			//std::cout << "Critter wants cache id " << int(ic_idx) << std::endl;
			Image* image = ImageCache::instance()->getImage(ic_idx);
			if (!image)
				return;
			SDL_Surface *surface = image->getSurface();
			if (!surface)
				return;
			
			target.x += _d->m_xshift;
			target.y += _d->m_yshift;

			if( getPositionType() == PT_HEX_GRID ) {
				target.x -= surface->w/2  - _d->m_anim->getCurrentShiftX();
				target.y -= surface->h - _d->m_anim->getCurrentShiftY();
			} else {
				// not needed.
			};
			image->render(&target,m_surface);
			_d->m_anim->update(ticks);
		}
	};

	void Visual::setLayerType(Visual::layerType lt) {
		_d->m_layer_type = lt;
	};
	void Visual::setPositionType(Visual::positionType pt) {
		_d->m_position_type = pt;
	};
	void Visual::setVisualType(Visual::visualType vt) {
		_d->m_visual_type = vt;
	};

	Visual::layerType Visual::getLayerType() const {
		return _d->m_layer_type;
	};
	Visual::positionType Visual::getPositionType() const {
		return _d->m_position_type;
	};
	Visual::visualType Visual::getVisualType() const {
		return _d->m_visual_type;
	};


}//FIFE
