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
		SDL_Color old_color = m_font->getColor();
		if(m_font_color) {
			m_font->setColor(m_color.r, m_color.g, m_color.b, m_color.unused);
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
					Point p1 = Point(r.x-overdraw, r.y-overdraw);
					Point p2 = Point(r.x+r.w+overdraw, r.y-overdraw);
					Point p3 = Point(r.x+r.w+overdraw, r.y+r.h+overdraw);
					Point p4 = Point(r.x-overdraw, r.y+r.h+overdraw);

					if(m_background) {
						m_renderbackend->drawQuad(p1, p2, p3, p4, m_backcolor.r, m_backcolor.g, m_backcolor.b, m_backcolor.unused);
					}

					if(m_backborder) {
						m_renderbackend->drawLine(p1, p2, m_backbordercolor.r, m_backbordercolor.g, m_backbordercolor.b, m_backbordercolor.unused);
						m_renderbackend->drawLine(p2, p3, m_backbordercolor.r, m_backbordercolor.g, m_backbordercolor.b, m_backbordercolor.unused);
						m_renderbackend->drawLine(p3, p4, m_backbordercolor.r, m_backbordercolor.g, m_backbordercolor.b, m_backbordercolor.unused);
						m_renderbackend->drawLine(p4, p1, m_backbordercolor.r, m_backbordercolor.g, m_backbordercolor.b, m_backbordercolor.unused);
					}
				}
				img->render(r);
			}
		}
		if(m_font_color) {
			m_font->setColor(old_color.r, old_color.g, old_color.b, old_color.unused);
		}
	}

	void FloatingTextRenderer::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
		m_color.unused = a;

		m_font_color = true;
	}

	void FloatingTextRenderer::setDefaultBackground(Uint8 br, Uint8 bg, Uint8 bb, Uint8 ba) {
		m_backcolor.r = br;
		m_backcolor.g = bg;
		m_backcolor.b = bb;
		m_backcolor.unused = ba;

		m_background = true;
	}

	void FloatingTextRenderer::setDefaultBorder(Uint8 bbr, Uint8 bbg, Uint8 bbb, Uint8 bba) {
		m_backbordercolor.r = bbr;
		m_backbordercolor.g = bbg;
		m_backbordercolor.b = bbb;
		m_backbordercolor.unused = bba;

		m_backborder = true;
	}

	void FloatingTextRenderer::resetDefaultBackground() {
		m_background = false;
	}

	void FloatingTextRenderer::resetDefaultBorder() {
		m_backborder = false;
	}

	FloatingTextRenderer* FloatingTextRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<FloatingTextRenderer*>(cnt->getRenderer("FloatingTextRenderer"));
	}
}
