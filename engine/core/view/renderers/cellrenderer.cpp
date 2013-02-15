/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
 *   http://www.fifengine.net                                               *
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
#include "video/imagemanager.h"
#include "video/fonts/ifont.h"
#include "util/math/fife_math.h"
#include "util/log/logger.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "model/structures/cell.h"
#include "model/structures/cellcache.h"
#include "pathfinder/route.h"

#include "view/camera.h"
#include "cellrenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);

	CellRenderer::CellRenderer(RenderBackend* renderbackend, int32_t position):
		RendererBase(renderbackend, position) {
		setEnabled(false);
		m_blockerColor.r = 255;
		m_blockerColor.g = 0;
		m_blockerColor.b = 0;
		m_pathColor.r = 0;
		m_pathColor.g = 0;
		m_pathColor.b = 255;
		m_blockingEnabled = false;
		m_fowEnabled = false;
		m_pathVisualEnabled = false;
		m_targetRenderer = new TargetRenderer(m_renderbackend);
		m_fowLayer = NULL;
		m_font = NULL;

	}

	CellRenderer::CellRenderer(const CellRenderer& old):
		RendererBase(old),
		m_blockerColor(old.m_blockerColor),
		m_pathColor(old.m_pathColor){
		setEnabled(false);
		m_blockingEnabled = false;
		m_fowEnabled = false;
		m_pathVisualEnabled = false;
		m_targetRenderer = new TargetRenderer(m_renderbackend);
		m_fowLayer = NULL;
		m_font = NULL;
	}

	RendererBase* CellRenderer::clone() {
		return new CellRenderer(*this);
	}

	CellRenderer::~CellRenderer() {
		delete m_targetRenderer;
	}

	CellRenderer* CellRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<CellRenderer*>(cnt->getRenderer("CellRenderer"));
	}

	std::string CellRenderer::getName() {
		return "CellRenderer";
	}

	void CellRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, cannot draw grid");
			return;
		}
		CellCache* cache = layer->getCellCache();
		if (!cache) {
			FL_WARN(_log, "No cellcache on layer created, cannot draw cells");
			return;
		}

		const bool fow = m_fowEnabled && (m_fowLayer == layer);
		bool fow_update = fow && (cam->isUpdated() || cache->isUpdated());
		if (!m_fowImage.get() && fow) {
			createFowMap(cam, layer);
		}

		const bool render_costs = (!m_visualCosts.empty() && m_font);
		const bool zoomed = !Mathd::Equal(1.0, cam->getZoom());

		Rect layerView = cam->getLayerViewPort(layer);
		std::vector<Cell*> cells = cache->getCellsInRect(layerView);
		std::vector<Cell*>::iterator cit = cells.begin();
		for (; cit != cells.end(); ++cit) {
			if (m_blockingEnabled) {
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
						m_renderbackend->drawLine(pt1, pt2, m_blockerColor.r, m_blockerColor.g, m_blockerColor.b);
						pt1 = pt2;
					}
					m_renderbackend->drawLine(pt2, Point(firstpt.x, firstpt.y), m_blockerColor.r, m_blockerColor.g, m_blockerColor.b);
					ScreenPoint spt1 = cam->toScreenCoordinates(cg->toMapCoordinates(vertices[0]));
					Point pt3(spt1.x, spt1.y);
					ScreenPoint spt2 = cam->toScreenCoordinates(cg->toMapCoordinates(vertices[halfind]));
					Point pt4(spt2.x, spt2.y);
					m_renderbackend->drawLine(pt3, pt4, m_blockerColor.r, m_blockerColor.g, m_blockerColor.b);
				}
			}
				
			if (render_costs) {
				bool match = false;
				double cost;
				std::set<std::string>::iterator cost_it = m_visualCosts.begin();
				for (; cost_it != m_visualCosts.end(); ++cost_it) {
					std::vector<std::string> cell_costs = cache->getCellCosts(*cit);
					std::vector<std::string>::iterator cc_it = cell_costs.begin();
					for (; cc_it != cell_costs.end(); ++cc_it) {
						if (*cc_it == *cost_it) {
							match = true;
							cost = cache->getCost(*cost_it);
							break;
						}
					}
					if (match) {
						break;
					}
				}
				if (match) {
					Location loc(layer);
					loc.setLayerCoordinates((*cit)->getLayerCoordinates());
					ScreenPoint drawpt = cam->toScreenCoordinates(loc.getMapCoordinates());

					std::stringstream stream;
					stream << cost;
					Image* img = m_font->getAsImage(stream.str());
					
					Rect r;
					if (zoomed) {
						double zoom = cam->getZoom();
						r.x = drawpt.x - (img->getWidth()/2) * zoom;
						r.y = drawpt.y - (img->getHeight()/2) * zoom;
						r.w = img->getWidth() * zoom;
						r.h = img->getHeight() * zoom;
						img->render(r);
					} else {
						r.x = drawpt.x - img->getWidth()/2;
						r.y = drawpt.y - img->getHeight()/2;
						r.w = img->getWidth();
						r.h = img->getHeight();
						img->render(r);
					}
				}
			}

			if (fow_update) {
				ScreenPoint sp = cam->toScreenCoordinates(cg->toMapCoordinates(
					FIFE::intPt2doublePt((*cit)->getLayerCoordinates())));
				CellVisualEffect cve = (*cit)->getFoWType();
				if (cve == CELLV_CONCEALED) {
					if (m_concealImage.get()) {
						addConcealImageToMap(cam, Point(sp.x, sp.y), m_concealImage);
					}
				} else if (cve == CELLV_MASKED) {
					if (m_maskImage.get()) {
						addMaskImageToMap(cam, Point(sp.x, sp.y), m_maskImage);
					}
				}
			}
		}

		if (m_pathVisualEnabled && !m_visualPaths.empty()) {
			std::vector<Instance*>::iterator it = m_visualPaths.begin();
			for (; it != m_visualPaths.end(); ++it) {
				Route* route = (*it)->getRoute();
				if (route) {
					Path path = route->getPath();
					if (!path.empty()) {
						Path::iterator pit = path.begin();
						for (; pit != path.end(); ++pit) {
							if ((*pit).getLayer() != layer) {
								continue;
							}
							std::vector<ExactModelCoordinate> vertices;
							cg->getVertices(vertices, (*pit).getLayerCoordinates());
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
								m_renderbackend->drawLine(pt1, pt2, m_pathColor.r, m_pathColor.g, m_pathColor.b);
								pt1 = pt2;
							}
							m_renderbackend->drawLine(pt2, Point(firstpt.x, firstpt.y), m_pathColor.r, m_pathColor.g, m_pathColor.b);
						}
					}
				}
			}
		}

		if (fow) {
			if (fow_update) {
				updateFowMap();
			}
			m_fowImage.get()->render(cam->getViewPort());
		}
	}

	void CellRenderer::setBlockerColor(uint8_t r, uint8_t g, uint8_t b) {
		m_blockerColor.r = r;
		m_blockerColor.g = g;
		m_blockerColor.b = b;
	}

	void CellRenderer::setPathColor(uint8_t r, uint8_t g, uint8_t b) {
		m_pathColor.r = r;
		m_pathColor.g = g;
		m_pathColor.b = b;
	}

	void CellRenderer::setFogOfWarLayer(Layer* layer) {
		m_fowLayer = layer;
	}
	
	void CellRenderer::setEnabledBlocking(bool enabled) {
		m_blockingEnabled = enabled;
	}
	
	bool CellRenderer::isEnabledBlocking() {
		return m_blockingEnabled;
	}

	void CellRenderer::setEnabledFogOfWar(bool enabled) {
		m_fowEnabled = enabled;
	}
	
	bool CellRenderer::isEnabledFogOfWar() {
		return m_fowEnabled;
	}

	void CellRenderer::setEnabledPathVisual(bool enabled) {
		m_pathVisualEnabled = enabled;
	}

	bool CellRenderer::isEnabledPathVisual() {
		return m_pathVisualEnabled;
	}

	void CellRenderer::createFowMap(Camera* cam, Layer* layer) {
		Rect view = cam->getViewPort();
		m_fowImage = ImageManager::instance()->loadBlank("virtual_fow_map", view.w, view.h);
		m_fowTarget = m_targetRenderer->createRenderTarget(m_fowImage);
		m_targetRenderer->setRenderTarget("virtual_fow_map", true, 0);
		Point n1(0, 0);
		Point n2(0, view.h);
		Point n3(view.w, view.h);
		Point n4(view.w, 0);
		m_fowTarget->addQuad("a_quad", n1, n2, n3, n4, 0, 0, 0, 0);
	}


	void CellRenderer::setConcealImage(ImagePtr image) {
		m_concealImage = image;
	}

	void CellRenderer::setMaskImage(ImagePtr image) {
		m_maskImage = image;
	}

	void CellRenderer::addConcealImageToMap(Camera* cam, Point center, ImagePtr image) {
		if (Mathd::Equal(cam->getZoom(), 1.0)) {
			m_fowTarget->addImage("c_img", center, image);
		} else {
			int32_t w = round(image.get()->getWidth() * cam->getZoom());
			int32_t h = round(image.get()->getHeight() * cam->getZoom());
			m_fowTarget->resizeImage("c_img", center, image, w, h);
		}
	}

	void CellRenderer::addMaskImageToMap(Camera* cam, Point center, ImagePtr image) {
		if (Mathd::Equal(cam->getZoom(), 1.0)) {
			m_fowTarget->addImage("b_img", center, image);
		} else {
			int32_t w = round(image.get()->getWidth() * cam->getZoom());
			int32_t h = round(image.get()->getHeight() * cam->getZoom());
			m_fowTarget->resizeImage("b_img", center, image, w, h);
		}
	}
	
	Layer* CellRenderer::getFowLayer() {
		return m_fowLayer;
	}

	void CellRenderer::setFont(IFont* font) {
		m_font = font;
	}

	IFont* CellRenderer::getFont() {
		return m_font;
	}

	void CellRenderer::updateFowMap() {
		m_targetRenderer->setRenderTarget("virtual_fow_map", true, 0);
		m_targetRenderer->render();
		m_fowTarget->removeAll("b_img");
		m_fowTarget->removeAll("c_img");
	}

	void CellRenderer::addPathVisual(Instance* instance) {
		m_visualPaths.push_back(instance);
	}

	void CellRenderer::removePathVisual(Instance* instance) {
		for (std::vector<Instance*>::iterator it = m_visualPaths.begin();
			it != m_visualPaths.end(); ++it) {

			if (*it == instance) {
				m_visualPaths.erase(it);
				break;
			}
		}
	}

	void CellRenderer::setEnabledCost(const std::string& costId, bool enabled) {
		if (enabled) {
			m_visualCosts.insert(costId);
		} else {
			m_visualCosts.erase(costId);
		}
	}
	
	bool CellRenderer::isEnabledCost(const std::string& costId) {
		std::set<std::string>::iterator it = m_visualCosts.find(costId);
		if (it != m_visualCosts.end()) {
			return true;
		}
		return false;
	}
}
