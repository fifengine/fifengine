/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"
#include "video/image.h"
#include "video/imagepool.h"
#include "video/fonts/abstractfont.h"
#include "util/math/fife_math.h"
#include "util/log/logger.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/action.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view/camera.h"
#include "view/visual.h"
#include "coordinaterenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);

	CoordinateRenderer::CoordinateRenderer(RenderBackend* renderbackend, int position, AbstractFont* font):
		RendererBase(renderbackend, position),
		m_layer_area(),
		m_tmploc(),
		m_c(),
		m_font(font) {
		setEnabled(false);
		m_font_color = false;
		m_color = m_font->getColor();
	}

 	CoordinateRenderer::CoordinateRenderer(const CoordinateRenderer& old):
		RendererBase(old),
		m_layer_area(),
		m_tmploc(),
		m_c(),
		m_font(old.m_font),
		m_font_color(old.m_font_color),
		m_color(old.m_color) {
		setEnabled(false);
	}

	RendererBase* CoordinateRenderer::clone() {
		return new CoordinateRenderer(*this);
	}
	
	CoordinateRenderer::~CoordinateRenderer() {
	}

	CoordinateRenderer* CoordinateRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<CoordinateRenderer*>(cnt->getRenderer("CoordinateRenderer"));
	}

	void CoordinateRenderer::adjustLayerArea() {
		m_tmploc.setMapCoordinates(m_c);
		ModelCoordinate c = m_tmploc.getLayerCoordinates();
		m_layer_area.x = std::min(c.x, m_layer_area.x);
		m_layer_area.w = std::max(c.x, m_layer_area.w);
		m_layer_area.y = std::min(c.y, m_layer_area.y);
		m_layer_area.h = std::max(c.y, m_layer_area.h);
	}

	const int MIN_COORD = -9999999;
	const int MAX_COORD = 9999999;
	void CoordinateRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		m_layer_area.x = MAX_COORD;
		m_layer_area.y = MAX_COORD;
		m_layer_area.w = MIN_COORD;
		m_layer_area.h = MIN_COORD;

		m_tmploc.setLayer(layer);
		Rect cv = cam->getViewPort();
		m_c = cam->toMapCoordinates(ScreenPoint(cv.x, cv.y), false);
		adjustLayerArea();
		m_c = cam->toMapCoordinates(ScreenPoint(cv.x+cv.w, cv.y), false);
		adjustLayerArea();
		m_c = cam->toMapCoordinates(ScreenPoint(cv.x, cv.y+cv.h), false);
		adjustLayerArea();
		m_c = cam->toMapCoordinates(ScreenPoint(cv.x+cv.w, cv.y+cv.h), false);
		adjustLayerArea();

		Rect r = Rect();
		SDL_Color old_color = m_font->getColor();
		if(old_color.r != m_color.r || old_color.g != m_color.g || old_color.b != m_color.b) {
			m_font->setColor(m_color.r, m_color.g, m_color.b);
			m_font_color = true;
		}
		for (int x = m_layer_area.x-1; x < m_layer_area.w+1; x++) {
			for (int y = m_layer_area.y-1; y < m_layer_area.h+1; y++) {
				ModelCoordinate mc(x, y);
				m_tmploc.setLayerCoordinates(mc);
				ScreenPoint drawpt = cam->toScreenCoordinates(m_tmploc.getMapCoordinates());
				if ((drawpt.x >= cv.x) && (drawpt.x <= cv.x + cv.w) &&
				    (drawpt.y >= cv.y) && (drawpt.y <= cv.y + cv.h)) {
					std::stringstream ss;
					ss << mc.x <<","<< mc.y;
					Image * img = m_font->getAsImage(ss.str());
					r.x = drawpt.x;
					r.y = drawpt.y;
					r.w = img->getWidth();
					r.h = img->getHeight();
					img->render(r);
				}
			}
		}
		if(m_font_color) {
			m_font->setColor(old_color.r, old_color.g, old_color.b);
			m_font_color = false;
		}
	}

	void CoordinateRenderer::setColor(Uint8 r, Uint8 g, Uint8 b) {
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
	}
}
