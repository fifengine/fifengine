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
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "model/structures/cell.h"
#include "model/structures/cellcache.h"

#include "view/camera.h"
#include "blockinginforenderer.h"


namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIEWVIEW);

	BlockingInfoRenderer::BlockingInfoRenderer(RenderBackend* renderbackend, int32_t position):
		RendererBase(renderbackend, position) {
		setEnabled(false);
		m_color.r = 0;
		m_color.g = 255;
		m_color.b = 0;
	}

	BlockingInfoRenderer::BlockingInfoRenderer(const BlockingInfoRenderer& old):
		RendererBase(old),
		m_color(old.m_color) {
		setEnabled(false);
	}

	RendererBase* BlockingInfoRenderer::clone() {
		return new BlockingInfoRenderer(*this);
	}

	BlockingInfoRenderer::~BlockingInfoRenderer() {
	}

	BlockingInfoRenderer* BlockingInfoRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<BlockingInfoRenderer*>(cnt->getRenderer("BlockingInfoRenderer"));
	}

	void BlockingInfoRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, cannot draw grid");
			return;
		}

		Rect cv = cam->getViewPort();
		CellCache* cache = layer->getCellCache();
		if (cache) {
			const std::vector<std::vector<Cell*> >& cells = cache->getCells();
			std::vector<std::vector<Cell*> >::const_iterator it = cells.begin();
			for (; it != cells.end(); ++it) {
				std::vector<Cell*>::const_iterator cit = (*it).begin();
				for (; cit != (*it).end(); ++cit) {
					ExactModelCoordinate emc = FIFE::intPt2doublePt((*cit)->getLayerCoordinates());
					ScreenPoint sp = cam->toScreenCoordinates(cg->toMapCoordinates(emc));
					// if it is not in cameras view continue
					if (sp.x < cv.x || sp.x > cv.x + cv.w ||
						sp.y < cv.y || sp.y > cv.y + cv.h) {
						continue;
					}
					if ((*cit)->getCellType() != CTYPE_NO_BLOCKER) {
						std::vector<ExactModelCoordinate> vertices;
						cg->getVertices(vertices, (*cit)->getLayerCoordinates());
						std::vector<ExactModelCoordinate>::const_iterator it = vertices.begin();
						int32_t halfind = vertices.size() / 2;
						ScreenPoint firstpt = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
						Point pt1(firstpt.x, firstpt.y);
						Point pt2;
						++it;
						for (; it != vertices.end(); it++) {
							ScreenPoint pts = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
							pt2.x = pts.x;
							pt2.y = pts.y;
							m_renderbackend->drawLine(pt1, pt2, m_color.r, m_color.g, m_color.b);
							pt1 = pt2;
						}
						m_renderbackend->drawLine(pt2, Point(firstpt.x, firstpt.y), m_color.r, m_color.g, m_color.b);
						ScreenPoint spt1 = cam->toScreenCoordinates(cg->toMapCoordinates(vertices[0]));
						Point pt3(spt1.x, spt1.y);
						ScreenPoint spt2 = cam->toScreenCoordinates(cg->toMapCoordinates(vertices[halfind]));
						Point pt4(spt2.x, spt2.y);
						m_renderbackend->drawLine(pt3, pt4, m_color.r, m_color.g, m_color.b);
					}
				}
			}
		} else {
			RenderList::const_iterator instance_it = instances.begin();
			for (;instance_it != instances.end(); ++instance_it) {
				Instance* instance = (*instance_it)->instance;
				if (!instance->getObject()->isBlocking() || !instance->isBlocking()) {
					continue;
				}
				std::vector<ExactModelCoordinate> vertices;
				cg->getVertices(vertices, instance->getLocationRef().getLayerCoordinates());
				std::vector<ExactModelCoordinate>::const_iterator it = vertices.begin();
				int32_t halfind = vertices.size() / 2;
				ScreenPoint firstpt = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
				Point pt1(firstpt.x, firstpt.y);
				Point pt2;
				++it;
				for (; it != vertices.end(); it++) {
					ScreenPoint pts = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
					pt2.x = pts.x;
					pt2.y = pts.y;
					m_renderbackend->drawLine(pt1, pt2, m_color.r, m_color.g, m_color.b);
					pt1 = pt2;
				}
				m_renderbackend->drawLine(pt2, Point(firstpt.x, firstpt.y), m_color.r, m_color.g, m_color.b);
				ScreenPoint spt1 = cam->toScreenCoordinates(cg->toMapCoordinates(vertices[0]));
				Point pt3(spt1.x, spt1.y);
				ScreenPoint spt2 = cam->toScreenCoordinates(cg->toMapCoordinates(vertices[halfind]));
				Point pt4(spt2.x, spt2.y);
				m_renderbackend->drawLine(pt3, pt4, m_color.r, m_color.g, m_color.b);
			}
		}
	}

	void BlockingInfoRenderer::setColor(uint8_t r, uint8_t g, uint8_t b) {
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
	}
}
