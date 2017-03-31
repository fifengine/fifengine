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
#include "util/math/fife_math.h"
#include "util/log/logger.h"
#include "video/fonts/ifont.h"
#include "video/image.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view/visual.h"
#include "view/camera.h"
#include "floatingtextrenderer.h"


namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIEWVIEW);

	FloatingTextRenderer::FloatingTextRenderer(RenderBackend* renderbackend, int32_t position):
		RendererBase(renderbackend, position),
		m_font(0),
		m_fontColor(false),
		m_background(false),
		m_backborder(false) {
		setEnabled(false);
	}

 	FloatingTextRenderer::FloatingTextRenderer(const FloatingTextRenderer& old):
		RendererBase(old),
		m_font(old.m_font),
		m_fontColor(old.m_fontColor),
		m_color(old.m_color),
		m_background(old.m_background),
		m_backborder(old.m_backborder) {
		setEnabled(false);
	}

	RendererBase* FloatingTextRenderer::clone() {
		return new FloatingTextRenderer(*this);
	}

	FloatingTextRenderer::~FloatingTextRenderer() {
	}

	FloatingTextRenderer* FloatingTextRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<FloatingTextRenderer*>(cnt->getRenderer("FloatingTextRenderer"));
	}

	void FloatingTextRenderer::removeActiveLayer(Layer* layer) {
		RendererBase::removeActiveLayer(layer);
		std::map<Layer*, std::map<Instance*, Data> >::iterator it = m_bufferMap.find(layer);
		if (it != m_bufferMap.end()) {
			m_bufferMap.erase(it);
		}
	}

	void FloatingTextRenderer::clearActiveLayers() {
		RendererBase::clearActiveLayers();
		m_bufferMap.clear();
	}

	void FloatingTextRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		if (!m_font) {
			//no font selected.. nothing to render
			return;
		}

		RenderList::const_iterator instance_it = instances.begin();
		SDL_Color old_color = m_font->getColor();
		if(m_fontColor) {
			m_font->setColor(m_color.r, m_color.g, m_color.b, m_color.a);
		}
		std::map<Instance*, Data>& instanceMap = m_bufferMap[layer];
		instanceMap.clear();
		for (;instance_it != instances.end(); ++instance_it) {
			Instance* instance = (*instance_it)->instance;
			const std::string* saytext = instance->getSayText();
			if (saytext) {
				const Rect& ir = (*instance_it)->dimensions;
				Image* img = m_font->getAsImageMultiline(*saytext);
				Rect r;
				r.x = (ir.x + ir.w/2) - img->getWidth()/2; /// the center of the text rect is always aligned to the instance's rect center.
				r.y = ir.y- img->getHeight(); /// make the text rect floating higher than the instance.
				r.w = img->getWidth();
				r.h = img->getHeight();
				// Without this check it can happen that changeRenderInfos() call produces an out_of_range error
				// because the image rendering can be skipped, if it's not on the screen.
				// The result is that it tried to modify more objects as exist.
				if (r.right() < 0 || r.x > static_cast<int32_t>(m_renderbackend->getWidth()) || 
					r.bottom() < 0 || r.y > static_cast<int32_t>(m_renderbackend->getHeight())) {
					continue;
				}

				Data& data = instanceMap[instance];
				data.image = img;
				data.imageRec = r;

				if(m_background || m_backborder) {
					const int32_t overdraw = 5;
					Point p = Point(r.x-overdraw, r.y-overdraw);
					data.backgroundRec.x = p.x;
					data.backgroundRec.y = p.y;
					data.backgroundRec.w = r.w + 2 * overdraw;
					data.backgroundRec.h = r.h + 2 * overdraw;
				}
			}
		}
		if(m_fontColor) {
			m_font->setColor(old_color.r, old_color.g, old_color.b, old_color.a);
		}
		renderBuffer(layer);
	}

	void FloatingTextRenderer::setFont(IFont* font) {
		m_font = font;
		m_bufferMap.clear();
	}

	void FloatingTextRenderer::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
		m_color.a = a;
		m_fontColor = true;
		m_bufferMap.clear();
	}

	void FloatingTextRenderer::setBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		m_backcolor.r = r;
		m_backcolor.g = g;
		m_backcolor.b = b;
		m_backcolor.a = a;
		m_background = true;
	}

	void FloatingTextRenderer::setBorder(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		m_backbordercolor.r = r;
		m_backbordercolor.g = g;
		m_backbordercolor.b = b;
		m_backbordercolor.a = a;
		m_backborder = true;
	}

	void FloatingTextRenderer::resetBackground() {
		m_background = false;
	}

	void FloatingTextRenderer::resetBorder() {
		m_backborder = false;
	}

	void FloatingTextRenderer::renderBuffer(Layer* layer) {
		std::map<Instance*, Data>& instanceMap = m_bufferMap[layer];
		std::map<Instance*, Data>::iterator it = instanceMap.begin();
		uint16_t elements = 0;
		for (; it != instanceMap.end(); ++it) {
			Data& data = (*it).second;
			Rect& rec = data.backgroundRec;

			if (m_background) {
				m_renderbackend->fillRectangle(Point(rec.x, rec.y), rec.w, rec.h, m_backcolor.r, m_backcolor.g, m_backcolor.b, m_backcolor.a);
				++elements;
			}
			if (m_backborder) {
				m_renderbackend->drawRectangle(Point(rec.x, rec.y), rec.w, rec.h, m_backbordercolor.r, m_backbordercolor.g, m_backbordercolor.b, m_backbordercolor.a);
				++elements;
			}
			rec = data.imageRec;
			data.image->render(rec);
			++elements;
		}
		if (m_renderbackend->getLightingModel() > 0) {
			m_renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, elements, 4, 5, false, true, 255, REPLACE, ALWAYS);
		}
	}
}
