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
#include "util/math/fife_math.h"
#include "util/log/logger.h"
#include "video/fonts/abstractfont.h"
#include "video/image.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view/visual.h"
#include "view/camera.h"
#include "floatingtextrenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);


	FloatingTextRenderer::FloatingTextRenderer(RenderBackend* renderbackend, int position, AbstractFont* font):
		RendererBase(renderbackend, position),
		m_renderbackend(renderbackend),
		m_font(font) {
		setEnabled(false);
		m_font_color = false;
		m_color = m_font->getColor();
	}

 	FloatingTextRenderer::FloatingTextRenderer(const FloatingTextRenderer& old):
		RendererBase(old),
		m_renderbackend(old.m_renderbackend),
		m_font(old.m_font),
		m_font_color(old.m_font_color),
		m_color(old.m_color) {
		setEnabled(false);
		m_font_color = m_background = m_backborder = false;
	}

	RendererBase* FloatingTextRenderer::clone() {
		return new FloatingTextRenderer(*this);
	}

	FloatingTextRenderer::~FloatingTextRenderer() {
	}

	void FloatingTextRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		if (!m_font) {
			return;
		}

		RenderList::const_iterator instance_it = instances.begin();
		const std::string* saytext = NULL;
		unsigned int lm = m_renderbackend->getLightingModel();
		SDL_Color old_color = m_font->getColor();
		if(m_font_color) {
			m_font->setColor(m_color.r, m_color.g, m_color.b, m_color.unused);
		}
		if(lm != 0) {
			m_renderbackend->disableLighting();
			m_renderbackend->setStencilTest(255, 2, 7);
			m_renderbackend->setAlphaTest(0.0);
		}
		for (;instance_it != instances.end(); ++instance_it) {
			Instance* instance = (*instance_it)->instance;
			saytext = instance->getSayText();
			if (saytext) {
				const Rect& ir = (*instance_it)->dimensions;
				Image* img = m_font->getAsImageMultiline(*saytext);
				Rect r;
				r.x = (ir.x + ir.w/2) - img->getWidth()/2; /// the center of the text rect is always aligned to the instance's rect center.
				r.y = ir.y- img->getHeight(); /// make the text rect floating higher than the instance.
				r.w = img->getWidth();
				r.h = img->getHeight();

				if(m_background || m_backborder) {
					const int overdraw = 5;

					Point p = Point(r.x-overdraw, r.y-overdraw);

					if(m_background) {
						m_renderbackend->fillRectangle(p, r.w+2*overdraw, r.h+2*overdraw, m_backcolor.r, m_backcolor.g, m_backcolor.b, m_backcolor.unused);
					}

					if(m_backborder) {
						m_renderbackend->drawRectangle(p, r.w+2*overdraw, r.h+2*overdraw, m_backbordercolor.r, m_backbordercolor.g, m_backbordercolor.b, m_backbordercolor.unused);
					}
				}
				img->render(r);
			}
		}
		if(lm != 0) {
			m_renderbackend->disableAlphaTest();
			m_renderbackend->disableStencilTest();
			m_renderbackend->enableLighting();
		}
		if(m_font_color) {
			m_font->setColor(old_color.r, old_color.g, old_color.b, old_color.unused);
		}
	}

	void FloatingTextRenderer::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
		m_color.unused = a;

		m_font_color = true;
	}

	void FloatingTextRenderer::setBackground(uint8_t br, uint8_t bg, uint8_t bb, uint8_t ba) {
		m_backcolor.r = br;
		m_backcolor.g = bg;
		m_backcolor.b = bb;
		m_backcolor.unused = ba;

		m_background = true;
	}

	void FloatingTextRenderer::setBorder(uint8_t bbr, uint8_t bbg, uint8_t bbb, uint8_t bba) {
		m_backbordercolor.r = bbr;
		m_backbordercolor.g = bbg;
		m_backbordercolor.b = bbb;
		m_backbordercolor.unused = bba;

		m_backborder = true;
	}

	void FloatingTextRenderer::resetBackground() {
		m_background = false;
	}

	void FloatingTextRenderer::resetBorder() {
		m_backborder = false;
	}

	FloatingTextRenderer* FloatingTextRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<FloatingTextRenderer*>(cnt->getRenderer("FloatingTextRenderer"));
	}
}
