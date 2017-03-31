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
		m_layerArea(),
		m_tmploc(),
		m_c(),
		m_font(0),
		m_zoom(true) {
		setEnabled(false);
	}

 	CoordinateRenderer::CoordinateRenderer(const CoordinateRenderer& old):
		RendererBase(old),
		m_layerArea(),
		m_tmploc(),
		m_c(),
		m_font(old.m_font),
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

	void CoordinateRenderer::setEnabled(bool enabled) {
		RendererBase::setEnabled(enabled);
		if (!enabled) {
			m_bufferMap.clear();
		}
	}

	void CoordinateRenderer::removeActiveLayer(Layer* layer) {
		RendererBase::removeActiveLayer(layer);
		std::map<Layer*, std::vector<Data> >::iterator it = m_bufferMap.find(layer);
		if (it != m_bufferMap.end()) {
			m_bufferMap.erase(it);
		}
	}

	void CoordinateRenderer::clearActiveLayers() {
		RendererBase::clearActiveLayers();
		m_bufferMap.clear();
	}

	void CoordinateRenderer::adjustLayerArea() {
		m_tmploc.setMapCoordinates(m_c);
		ModelCoordinate c = m_tmploc.getLayerCoordinates();
		m_layerArea.x = std::min(c.x, m_layerArea.x);
		m_layerArea.w = std::max(c.x, m_layerArea.w);
		m_layerArea.y = std::min(c.y, m_layerArea.y);
		m_layerArea.h = std::max(c.y, m_layerArea.h);
	}

	const int32_t MIN_COORD = -9999999;
	const int32_t MAX_COORD = 9999999;
	void CoordinateRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		if (!m_font) {
			//no font selected.. nothing to render
			return;
		}

		if (!cam->isUpdated()) {
			if (m_bufferMap.find(layer) != m_bufferMap.end()) {
				renderBuffer(layer);
				return;
			}
		}

		const bool zoomed = (!Mathd::Equal(1.0, cam->getZoom()) && m_zoom);
		Rect cv = cam->getViewPort();

		m_tmploc.setLayer(layer);
		m_layerArea.x = MAX_COORD;
		m_layerArea.y = MAX_COORD;
		m_layerArea.w = MIN_COORD;
		m_layerArea.h = MIN_COORD;

		m_c = cam->toMapCoordinates(ScreenPoint(cv.x, cv.y), false);
		adjustLayerArea();
		m_c = cam->toMapCoordinates(ScreenPoint(cv.x+cv.w, cv.y), false);
		adjustLayerArea();
		m_c = cam->toMapCoordinates(ScreenPoint(cv.x, cv.y+cv.h), false);
		adjustLayerArea();
		m_c = cam->toMapCoordinates(ScreenPoint(cv.x+cv.w, cv.y+cv.h), false);
		adjustLayerArea();

		SDL_Color oldColor = m_font->getColor();
		bool fontColor = false;
		if(oldColor.r != m_color.r || oldColor.g != m_color.g || oldColor.b != m_color.b) {
			m_font->setColor(m_color.r, m_color.g, m_color.b);
			fontColor = true;
		}
		std::vector<Data>& data = m_bufferMap[layer];
		data.clear();
		Data dat;
		for (int32_t x = m_layerArea.x-1; x < m_layerArea.w+1; x++) {
			for (int32_t y = m_layerArea.y-1; y < m_layerArea.h+1; y++) {
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
					dat.rect.x = drawpt.x - ((imgx->getWidth() + imgc->getWidth() + imgy->getWidth())/2) * zoom;
					dat.rect.y = drawpt.y - (imgx->getHeight()/2) * zoom;
					dat.rect.w = imgx->getWidth() * zoom;
					dat.rect.h = imgx->getHeight() * zoom;
					dat.image = imgx;
					data.push_back(dat);

					dat.rect.x += dat.rect.w;
					dat.rect.w = imgc->getWidth() * zoom;
					dat.image = imgc;
					data.push_back(dat);

					dat.rect.x += dat.rect.w;
					dat.rect.w = imgy->getWidth() * zoom;
					dat.image = imgy;
					data.push_back(dat);
				} else {
					dat.rect.x = drawpt.x - (imgx->getWidth() + imgc->getWidth() + imgy->getWidth())/2;
					dat.rect.y = drawpt.y - imgx->getHeight()/2;
					dat.rect.w = imgx->getWidth();
					dat.rect.h = imgx->getHeight();
					dat.image = imgx;
					data.push_back(dat);

					dat.rect.x += dat.rect.w;
					dat.rect.w = imgc->getWidth();
					dat.image = imgc;
					data.push_back(dat);

					dat.rect.x += dat.rect.w;
					dat.rect.w = imgy->getWidth();
					dat.image = imgy;
					data.push_back(dat);
				}
			}
		}
		if (fontColor) {
			m_font->setColor(oldColor.r, oldColor.g, oldColor.b);
		}
		// render
		renderBuffer(layer);
	}

	void CoordinateRenderer::setColor(uint8_t r, uint8_t g, uint8_t b) {
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
		m_bufferMap.clear();
	}

	void CoordinateRenderer::setFont(IFont* font) {
		m_font = font;
		m_bufferMap.clear();
	}

	void CoordinateRenderer::setZoom(bool enable) {
		m_zoom = enable;
		m_bufferMap.clear();
	}

	void CoordinateRenderer::renderBuffer(Layer* layer) {
		std::vector<Data>& data = m_bufferMap[layer];
		std::vector<Data>::iterator it = data.begin();
		for (; it != data.end(); ++it) {
			(*it).image->render((*it).rect);
		}
	}
}
