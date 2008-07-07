/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include "video/renderbackend.h"
#include "util/math/fife_math.h"
#include "util/log/logger.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view/camera.h"
#include "geometricrenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);
	GeometricRenderer::LineInfo::LineInfo(Point p1, Point p2, int r, int g, int b):
		p1(p1),
		p2(p2),
		r(r),
		g(g),
		b(b) {
	}
	
	GeometricRenderer* GeometricRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<GeometricRenderer*>(cnt->getRenderer("GeometricRenderer"));
	}
	
	GeometricRenderer::GeometricRenderer(RenderBackend* renderbackend, int position):
		RendererBase(renderbackend, position),
		m_lines() {
		setEnabled(false);
	}

	GeometricRenderer::GeometricRenderer(const GeometricRenderer& old):
		RendererBase(old) {
		setEnabled(false);
	}

	RendererBase* GeometricRenderer::clone() {
		return new GeometricRenderer(*this);
	}

	GeometricRenderer::~GeometricRenderer() {
	}

	void GeometricRenderer::addLine(Point p1, Point p2, int r, int g, int b) {
		LineInfo info(p1, p2, r, g, b);
		m_lines.push_back(info);
	}

	void GeometricRenderer::removeAllLines() {
		m_lines.clear();
	}

	void GeometricRenderer::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances) {
		std::vector<LineInfo>::const_iterator lineinfo_it = m_lines.begin();
		for (;lineinfo_it != m_lines.end(); ++lineinfo_it) {
			LineInfo info = *lineinfo_it;
			m_renderbackend->drawLine(info.p1, info.p2, info.r, info.g, info.b);
		}
	}
}
