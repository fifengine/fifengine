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
#include "video/animation.h"
#include "video/fonts/ifont.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "util/math/fife_math.h"
#include "util/log/logger.h"
#include "util/time/timemanager.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/timeprovider.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view/camera.h"
#include "genericrenderer.h"


namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIEWVIEW);

	GenericRendererLineInfo::GenericRendererLineInfo(RendererNode n1, RendererNode n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a):
		GenericRendererElementInfo(),
		m_edge1(n1),
		m_edge2(n2),
		m_red(r),
		m_green(g),
		m_blue(b),
		m_alpha(a) {
	}
	void GenericRendererLineInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {
		Point p1 = m_edge1.getCalculatedPoint(cam, layer);
		Point p2 = m_edge2.getCalculatedPoint(cam, layer);
		if(m_edge1.getLayer() == layer) {
			renderbackend->drawLine(p1, p2, m_red, m_green, m_blue, m_alpha);
			if (renderbackend->getLightingModel() > 0) {
				renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, false, 0, KEEP, ALWAYS);
			}
		}
	}

	GenericRendererPointInfo::GenericRendererPointInfo(RendererNode anchor, uint8_t r, uint8_t g, uint8_t b, uint8_t a):
		GenericRendererElementInfo(),
		m_anchor(anchor),
		m_red(r),
		m_green(g),
		m_blue(b),
		m_alpha(a) {
	}
	void GenericRendererPointInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
			renderbackend->putPixel(p.x, p.y, m_red, m_green, m_blue, m_alpha);
			if (renderbackend->getLightingModel() > 0) {
				renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, false, 0, KEEP, ALWAYS);
			}
		}
	}

	GenericRendererTriangleInfo::GenericRendererTriangleInfo(RendererNode n1, RendererNode n2, RendererNode n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a):
		GenericRendererElementInfo(),
		m_edge1(n1),
		m_edge2(n2),
		m_edge3(n3),
		m_red(r),
		m_green(g),
		m_blue(b),
		m_alpha(a) {
	}
	void GenericRendererTriangleInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {
		Point p1 = m_edge1.getCalculatedPoint(cam, layer);
		Point p2 = m_edge2.getCalculatedPoint(cam, layer);
		Point p3 = m_edge3.getCalculatedPoint(cam, layer);
		if(m_edge1.getLayer() == layer) {
			renderbackend->drawTriangle(p1, p2, p3, m_red, m_green, m_blue, m_alpha);
			if (renderbackend->getLightingModel() > 0) {
				renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, false, 0, KEEP, ALWAYS);
			}
		}
	}

	GenericRendererQuadInfo::GenericRendererQuadInfo(RendererNode n1, RendererNode n2, RendererNode n3, RendererNode n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a):
		GenericRendererElementInfo(),
		m_edge1(n1),
		m_edge2(n2),
		m_edge3(n3),
		m_edge4(n4),
		m_red(r),
		m_green(g),
		m_blue(b),
		m_alpha(a) {
	}
	void GenericRendererQuadInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {
		Point p1 = m_edge1.getCalculatedPoint(cam, layer);
		Point p2 = m_edge2.getCalculatedPoint(cam, layer);
		Point p3 = m_edge3.getCalculatedPoint(cam, layer);
		Point p4 = m_edge4.getCalculatedPoint(cam, layer);
		if(m_edge1.getLayer() == layer) {
			renderbackend->drawQuad(p1, p2, p3, p4, m_red, m_green, m_blue, m_alpha);
			if (renderbackend->getLightingModel() > 0) {
				renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, false, 0, KEEP, ALWAYS);
			}
		}
	}

	GenericRendererVertexInfo::GenericRendererVertexInfo(RendererNode center, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a):
		GenericRendererElementInfo(),
		m_center(center),
		m_size(size),
		m_red(r),
		m_green(g),
		m_blue(b),
		m_alpha(a) {
	}
	void GenericRendererVertexInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {
		Point p = m_center.getCalculatedPoint(cam, layer);
		if(m_center.getLayer() == layer) {
			renderbackend->drawVertex(p, m_size, m_red, m_green, m_blue, m_alpha);
			if (renderbackend->getLightingModel() > 0) {
				renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, false, 0, KEEP, ALWAYS);
			}
		}
	}

	GenericRendererImageInfo::GenericRendererImageInfo(RendererNode anchor, ImagePtr image, bool zoomed):
		GenericRendererElementInfo(),
		m_anchor(anchor),
		m_image(image),
		m_zoomed(zoomed) {
	}
	void GenericRendererImageInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {
		Point p = m_anchor.getCalculatedPoint(cam, layer, m_zoomed);
		if(m_anchor.getLayer() == layer) {
			Rect r;
			Rect viewport = cam->getViewPort();
			uint32_t width, height;
			if(m_zoomed) {
				width = static_cast<uint32_t>(round(m_image->getWidth() * cam->getZoom()));
				height = static_cast<uint32_t>(round(m_image->getHeight() * cam->getZoom()));
			}
			else {
				width = m_image->getWidth();
				height = m_image->getHeight();
			}
			r.x = p.x-width/2;
			r.y = p.y-height/2;
			r.w = width;
			r.h = height;
			if(r.intersects(viewport)) {
				m_image->render(r);
			}
		}
	}

	GenericRendererAnimationInfo::GenericRendererAnimationInfo(RendererNode anchor, AnimationPtr animation, bool zoomed):
		GenericRendererElementInfo(),
		m_anchor(anchor),
		m_animation(animation),
		m_start_time(TimeManager::instance()->getTime()),
		m_time_scale(1.0),
		m_zoomed(zoomed) {
	}
	void GenericRendererAnimationInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {
		Point p = m_anchor.getCalculatedPoint(cam, layer, m_zoomed);
		if(m_anchor.getLayer() == layer) {
			int32_t animtime = scaleTime(m_time_scale, TimeManager::instance()->getTime() - m_start_time) % m_animation->getDuration();
			ImagePtr img = m_animation->getFrameByTimestamp(animtime);
			Rect r;
			Rect viewport = cam->getViewPort();
			uint32_t width, height;
			if(m_zoomed) {
				width = static_cast<uint32_t>(round(img->getWidth() * cam->getZoom()));
				height = static_cast<uint32_t>(round(img->getHeight() * cam->getZoom()));
			} else {
				width = img->getWidth();
				height = img->getHeight();
			}
			r.x = p.x-width/2;
			r.y = p.y-height/2;
			r.w = width;
			r.h = height;
			if(r.intersects(viewport)) {
				img->render(r);
			}
		}
	}

	GenericRendererTextInfo::GenericRendererTextInfo(RendererNode anchor, IFont* font, std::string text, bool zoomed):
		GenericRendererElementInfo(),
		m_anchor(anchor),
		m_font(font),
		m_text(text),
		m_zoomed(zoomed) {
	}
	void GenericRendererTextInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {
		Point p = m_anchor.getCalculatedPoint(cam, layer, m_zoomed);
		if(m_anchor.getLayer() == layer) {
			Image* img = m_font->getAsImageMultiline(m_text);
			Rect r;
			Rect viewport = cam->getViewPort();
			uint32_t width, height;
			if(m_zoomed) {
				width = static_cast<uint32_t>(round(img->getWidth() * cam->getZoom()));
				height = static_cast<uint32_t>(round(img->getHeight() * cam->getZoom()));
			} else {
				width = img->getWidth();
				height = img->getHeight();
			}
			r.x = p.x-width/2;
			r.y = p.y-height/2;
			r.w = width;
			r.h = height;
			if(r.intersects(viewport)) {
				img->render(r);
				if (renderbackend->getLightingModel() > 0) {
					renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, false, 0, KEEP, ALWAYS);
				}
			}
		}
	}

	GenericRendererResizeInfo::GenericRendererResizeInfo(RendererNode anchor, ImagePtr image, int32_t width, int32_t height, bool zoomed):
		GenericRendererElementInfo(),
		m_anchor(anchor),
		m_image(image),
		m_width(width),
		m_height(height),
		m_zoomed(zoomed) {
	}
	void GenericRendererResizeInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {
		Point p = m_anchor.getCalculatedPoint(cam, layer, m_zoomed);
		if(m_anchor.getLayer() == layer) {
			Rect r;
			Rect viewport = cam->getViewPort();
			uint32_t width, height;
			if(m_zoomed) {
				width = static_cast<uint32_t>(round(m_width * cam->getZoom()));
				height = static_cast<uint32_t>(round(m_height * cam->getZoom()));
			} else {
				width = m_width;
				height = m_height;
			}
			r.x = p.x-width/2;
			r.y = p.y-height/2;
			r.w = width;
			r.h = height;
			if(r.intersects(viewport)) {
				m_image->render(r);
			}
		}
	}

	GenericRenderer* GenericRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<GenericRenderer*>(cnt->getRenderer("GenericRenderer"));
	}

	GenericRenderer::GenericRenderer(RenderBackend* renderbackend, int32_t position):
		RendererBase(renderbackend, position),
		m_groups() {
		setEnabled(false);
	}

	GenericRenderer::GenericRenderer(const GenericRenderer& old):
		RendererBase(old),
		m_groups() {
		setEnabled(false);
	}

	RendererBase* GenericRenderer::clone() {
		return new GenericRenderer(*this);
	}

	GenericRenderer::~GenericRenderer() {
	}
	void GenericRenderer::addLine(const std::string &group, RendererNode n1, RendererNode n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		GenericRendererElementInfo* info = new GenericRendererLineInfo(n1, n2, r, g, b, a);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addPoint(const std::string &group, RendererNode n, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		GenericRendererElementInfo* info = new GenericRendererPointInfo(n, r, g, b, a);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addTriangle(const std::string &group, RendererNode n1, RendererNode n2, RendererNode n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		GenericRendererElementInfo* info = new GenericRendererTriangleInfo(n1, n2, n3, r, g, b, a);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addQuad(const std::string &group, RendererNode n1, RendererNode n2, RendererNode n3, RendererNode n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		GenericRendererElementInfo* info = new GenericRendererQuadInfo(n1, n2, n3, n4, r, g, b, a);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addVertex(const std::string &group, RendererNode n, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		GenericRendererElementInfo* info = new GenericRendererVertexInfo(n, size, r, g, b, a);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addText(const std::string &group, RendererNode n, IFont* font, const std::string &text, bool zoomed) {
		GenericRendererElementInfo* info = new GenericRendererTextInfo(n, font, text, zoomed);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addImage(const std::string &group, RendererNode n, ImagePtr image, bool zoomed) {
		GenericRendererElementInfo* info = new GenericRendererImageInfo(n, image, zoomed);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addAnimation(const std::string &group, RendererNode n, AnimationPtr animation, bool zoomed) {
		GenericRendererElementInfo* info = new GenericRendererAnimationInfo(n, animation, zoomed);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::resizeImage(const std::string &group, RendererNode n, ImagePtr image, int32_t width, int32_t height, bool zoomed) {
		GenericRendererElementInfo* info = new GenericRendererResizeInfo(n, image, width, height, zoomed);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::removeAll(const std::string &group) {
		std::vector<GenericRendererElementInfo*>::const_iterator info_it = m_groups[group].begin();
		for (;info_it != m_groups[group].end(); ++info_it) {
			delete *info_it;
		}
		m_groups[group].clear();
		m_groups.erase(group);
	}
	// Remove all groups
	void GenericRenderer::removeAll() {
		std::map<std::string, std::vector<GenericRendererElementInfo*> >::iterator it = m_groups.begin();
		for (; it != m_groups.end(); ++it) {
			std::vector<GenericRendererElementInfo*>::const_iterator info_it = it->second.begin();
			for (;info_it != it->second.end(); ++info_it) {
				delete *info_it;
			}
		}
		m_groups.clear();
	}
	// Clear all groups
	void GenericRenderer::reset() {
		removeAll();
	}

	void GenericRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		std::map<std::string, std::vector<GenericRendererElementInfo*> >::iterator group_it = m_groups.begin();
		for(; group_it != m_groups.end(); ++group_it) {
			std::vector<GenericRendererElementInfo*>::const_iterator info_it = group_it->second.begin();
			for (;info_it != group_it->second.end(); ++info_it) {
				(*info_it)->render(cam, layer, instances, m_renderbackend);
			}
		}
	}
}
