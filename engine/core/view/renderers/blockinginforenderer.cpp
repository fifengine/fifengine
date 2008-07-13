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
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view/camera.h"
#include "view/view.h"
#include "blockinginforenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);

	BlockingInfoRenderer::BlockingInfoRenderer(RenderBackend* renderbackend, int position):
		RendererBase(renderbackend, position) {
		setEnabled(false);
	}

	BlockingInfoRenderer::BlockingInfoRenderer(const BlockingInfoRenderer& old):
		RendererBase(old) {
	}

	RendererBase* BlockingInfoRenderer::clone() {
		return new BlockingInfoRenderer(*this);
	}

	BlockingInfoRenderer::~BlockingInfoRenderer() {
	}

	void BlockingInfoRenderer::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances) {
		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, cannot draw grid");
			return;
		}
		
		Rect cv = cam->getViewPort();
		std::vector<Instance*>::const_iterator instance_it = instances.begin();
		for (;instance_it != instances.end(); ++instance_it) {
			Instance* instance = *instance_it;
			if (!instance->getObject()->isBlocking()) {
				continue;
			}
			std::vector<ExactModelCoordinate> vertices;
			cg->getVertices(vertices, instance->getLocationRef().getLayerCoordinates());
			int halfind = vertices.size() / 2;
			ScreenPoint spt1 = cam->toScreenCoordinates(cg->toMapCoordinates(vertices[0]));
			Point pt1(spt1.x, spt1.y);
			ScreenPoint spt2 = cam->toScreenCoordinates(cg->toMapCoordinates(vertices[halfind]));
			Point pt2(spt2.x, spt2.y);
			m_renderbackend->drawLine(pt1, pt2, 0, 255, 0);
		}
	}
}
