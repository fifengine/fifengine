/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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
			//renderbackend->drawVertex(p, m_size, m_red, m_green, m_blue, m_alpha);
			//renderbackend->drawCircle(p, m_size, m_red, m_green, m_blue, m_alpha);
			//return;
			//renderbackend->drawCircleSegment(p, m_size, 90, 180, m_red, m_green, m_blue, m_alpha);
			//return;
			/*renderbackend->drawFillCircleSegment(p, m_size, 0, 10, 255, 0, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 10, 20, 255, 10, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 20, 30, 255, 20, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 30, 40, 255, 30, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 40, 50, 255, 40, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 50, 60, 255, 50, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 60, 70, 255, 60, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 70, 80, 255, 70, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 80, 90, 255, 80, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 90, 100, 255, 90, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 100, 110, 255, 100, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 110, 120, 255, 110, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 120, 130, 255, 120, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 130, 140, 255, 130, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 140, 150, 255, 140, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 150, 160, 255, 150, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 160, 170, 255, 160, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 170, 180, 255, 170, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 180, 190, 255, 180, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 190, 200, 255, 190, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 200, 210, 255, 200, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 210, 220, 255, 210, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 220, 230, 255, 220, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 230, 240, 255, 230, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 240, 250, 255, 240, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 250, 260, 255, 250, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 260, 270, 255, 250, 10, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 270, 280, 255, 250, 20, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 280, 290, 255, 250, 30, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 290, 300, 255, 250, 40, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 300, 310, 255, 250, 50, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 310, 320, 255, 250, 60, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 320, 330, 255, 250, 70, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 330, 340, 255, 250, 80, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 340, 350, 255, 250, 90, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 350, 360, 255, 250, 100, m_alpha);*/

			/*renderbackend->drawFillCircleSegment(p, m_size, 0, 10, 0, 0, 0, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 10, 20, 7, 7, 7, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 20, 30, 14, 14, 14, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 30, 40, 21, 21, 21, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 40, 50, 28, 28, 28, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 50, 60, 35, 35, 35, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 60, 70, 42, 42, 42, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 70, 80, 49, 49, 49, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 80, 90, 56, 56, 56, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 90, 100, 63, 63, 63, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 100, 110, 70, 70, 70, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 110, 120, 77, 77, 77, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 120, 130, 84, 84, 84, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 130, 140, 91, 91, 91, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 140, 150, 98, 98, 98, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 150, 160, 105, 105, 105, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 160, 170, 112, 112, 112, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 170, 180, 119, 119, 119, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 180, 190, 126, 126, 126, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 190, 200, 133, 133, 133, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 200, 210, 140, 140, 140, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 210, 220, 147, 147, 147, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 220, 230, 154, 154, 154, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 230, 240, 161, 161, 161, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 240, 250, 168, 168, 168, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 250, 260, 175, 175, 175, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 260, 270, 182, 182, 182, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 270, 280, 189, 189, 189, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 280, 290, 196, 196, 196, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 290, 300, 203, 203, 203, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 300, 310, 210, 210, 210, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 310, 320, 217, 217, 217, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 320, 330, 224, 224, 224, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 330, 340, 231, 231, 231, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 340, 350, 238, 238, 238, m_alpha);
			renderbackend->drawFillCircleSegment(p, m_size, 350, 360, 245, 245, 245, m_alpha);*/

			//renderbackend->drawFillCircleSegment(p, m_size, 0, 90, 255, 0, 0, m_alpha);
			//renderbackend->drawFillCircleSegment(p, m_size, 90, 180, 0, 255, 0, m_alpha);
			//renderbackend->drawFillCircleSegment(p, m_size, 180, 270, 0, 0, 255, m_alpha);
			//renderbackend->drawFillCircleSegment(p, m_size, 270, 360, 255, 0, 255, m_alpha);
			//renderbackend->drawCircleSegment(p, m_size, 0, 359, 255, 0, 0, m_alpha);
			Point p2 = p;
			//p2.x += 100;
			p2.y -= 100;

			//Point p1line = p;
			//Point p2line = p;
			//p2line.x += 100;
			//p2line.y += 100;
			//renderbackend->drawThickLine(p1line, p2line, 30, 0, 0, 255, m_alpha);

			//renderbackend->drawLine(p1line, p2line, 0, 255, 0, m_alpha);

			std::vector<Point> points;
			points.push_back(p);
			
			Point p3 = p;
			p3.x += 100;
			p3.y -= 100;
			Point p4 = p;
			p4.x += 100;

			//p3.y -= 50;
			//points.push_back(p3);
			points.push_back(p2);
			points.push_back(p3);
			points.push_back(p4);
			renderbackend->drawBezier(points, 100, 12, 255, 0, 0, m_alpha);

			// draw points
			/*renderbackend->putPixel(p.x, p.y, 0, 0, 255, m_alpha);
			renderbackend->putPixel(p2.x, p2.y, 0, 0, 255, m_alpha);
			renderbackend->putPixel(p3.x, p3.y, 0, 0, 255, m_alpha);
			renderbackend->putPixel(p4.x, p4.y, 0, 0, 255, m_alpha);*/

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
