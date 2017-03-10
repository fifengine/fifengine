/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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
#include "video/fonts/ifont.h"
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
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIEWVIEW);

	CoordinateRenderer::CoordinateRenderer(RenderBackend* renderbackend, int32_t position):
		RendererBase(renderbackend, position),
		m_layer_area(),
		m_tmploc(),
		m_c(),
		m_font(0),
		m_font_color(false),
		m_zoom(true) {
		setEnabled(false);
	}

 	CoordinateRenderer::CoordinateRenderer(const CoordinateRenderer& old):
		RendererBase(old),
		m_layer_area(),
		m_tmploc(),
		m_c(),
		m_font(old.m_font),
		m_font_color(false),
		m_color(old.m_color),
		m_zoom(old.m_zoom) {
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

	const int32_t MIN_COORD = -9999999;
	const int32_t MAX_COORD = 9999999;
	void CoordinateRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		if (!m_font) {
			//no font selected.. nothing to render
			return;
		}
		Rect r = Rect();
		const bool zoomed = (!Mathd::Equal(1.0, cam->getZoom()) && m_zoom);
		Rect cv = cam->getViewPort();

		m_tmploc.setLayer(layer);
		m_layer_area.x = MAX_COORD;
		m_layer_area.y = MAX_COORD;
		m_layer_area.w = MIN_COORD;
		m_layer_area.h = MIN_COORD;

		m_c = cam->toMapCoordinates(ScreenPoint(cv.x, cv.y), false);
		adjustLayerArea();
		m_c = cam->toMapCoordinates(ScreenPoint(cv.x+cv.w, cv.y), false);
		adjustLayerArea();
		m_c = cam->toMapCoordinates(ScreenPoint(cv.x, cv.y+cv.h), false);
		adjustLayerArea();
		m_c = cam->toMapCoordinates(ScreenPoint(cv.x+cv.w, cv.y+cv.h), false);
		adjustLayerArea();

		SDL_Color old_color = m_font->getColor();
		if(old_color.r != m_color.r || old_color.g != m_color.g || old_color.b != m_color.b) {
			m_font->setColor(m_color.r, m_color.g, m_color.b);
			m_font_color = true;
		}
		for (int32_t x = m_layer_area.x-1; x < m_layer_area.w+1; x++) {
			for (int32_t y = m_layer_area.y-1; y < m_layer_area.h+1; y++) {
				ModelCoordinate mc(x, y);
				m_tmploc.setLayerCoordinates(mc);
				ScreenPoint drawpt = cam->toScreenCoordinates(m_tmploc.getMapCoordinates());
				if (drawpt.x < cv.x || drawpt.x > cv.x + cv.w ||
					drawpt.y < cv.y || drawpt.y > cv.y + cv.h) {
					continue;
				}

				// we split the stringstream in three images, because so the TextRenderPool can reuse the most images.
				// more to render but less images to generate
				std::stringstream sts;
				sts << mc.x;
				Image* imgx = m_font->getAsImage(sts.str());
				sts.str(",");
				Image* imgc = m_font->getAsImage(sts.str());
				sts.str("");
				sts << mc.y;
				Image* imgy = m_font->getAsImage(sts.str());

				if (zoomed) {
					double zoom = cam->getZoom();
					r.x = drawpt.x - ((imgx->getWidth() + imgc->getWidth() + imgy->getWidth())/2) * zoom;
					r.y = drawpt.y - (imgx->getHeight()/2) * zoom;
					r.w = imgx->getWidth() * zoom;
					r.h = imgx->getHeight() * zoom;
					imgx->render(r);
					r.x += r.w;
					r.w = imgc->getWidth() * zoom;
					imgc->render(r);
					r.x += r.w;
					r.w = imgy->getWidth() * zoom;
					imgy->render(r);
				} else {
					r.x = drawpt.x - (imgx->getWidth() + imgc->getWidth() + imgy->getWidth())/2;
					r.y = drawpt.y - imgx->getHeight()/2;
					r.w = imgx->getWidth();
					r.h = imgx->getHeight();
					imgx->render(r);
					r.x += r.w;
					r.w = imgc->getWidth();
					imgc->render(r);
					r.x += r.w;
					r.w = imgy->getWidth();
					imgy->render(r);
				}
			}
		}
		if(m_font_color) {
			m_font->setColor(old_color.r, old_color.g, old_color.b);
			m_font_color = false;
		}
	}

	void CoordinateRenderer::setColor(uint8_t r, uint8_t g, uint8_t b) {
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
	}
}
