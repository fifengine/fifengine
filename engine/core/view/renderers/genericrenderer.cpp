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
#include "video/imagepool.h"
#include "video/animation.h"
#include "video/animationpool.h"
#include "video/fonts/abstractfont.h"
#include "video/image.h"
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
	static Logger _log(LM_VIEWVIEW);

	GenericRendererNode::GenericRendererNode(Instance* attached_instance, const Location &relative_location, Layer* relative_layer, const Point &relative_point):
		m_instance(attached_instance),
		m_location(relative_location),
		m_layer(relative_layer),
		m_point(relative_point) {
	}
	GenericRendererNode::GenericRendererNode(Instance* attached_instance, const Location &relative_location, const Point &relative_point):
		m_instance(attached_instance),
		m_location(relative_location),
		m_layer(NULL),
		m_point(relative_point) {
	}
	GenericRendererNode::GenericRendererNode(Instance* attached_instance, Layer* relative_layer, const Point &relative_point):
		m_instance(attached_instance),
		m_location(NULL),
		m_layer(relative_layer),
		m_point(relative_point) {
	}
	GenericRendererNode::GenericRendererNode(Instance* attached_instance, const Point &relative_point):
		m_instance(attached_instance),
		m_location(NULL),
		m_layer(NULL),
		m_point(relative_point) {
	}
	GenericRendererNode::GenericRendererNode(const Location &attached_location, Layer* relative_layer, const Point &relative_point):
		m_instance(NULL),
		m_location(attached_location),
		m_layer(relative_layer),
		m_point(relative_point) {
	}
	GenericRendererNode::GenericRendererNode(const Location &attached_location, const Point &relative_point):
		m_instance(NULL),
		m_location(attached_location),
		m_layer(NULL),
		m_point(relative_point) {
	}
	GenericRendererNode::GenericRendererNode(Layer* attached_layer, const Point &relative_point):
		m_instance(NULL),
		m_location(NULL),
		m_layer(attached_layer),
		m_point(relative_point) {
	}
	GenericRendererNode::GenericRendererNode(const Point &attached_point):
		m_instance(NULL),
		m_location(NULL),
		m_layer(NULL),
		m_point(attached_point) {
	}
	GenericRendererNode::~GenericRendererNode() {
	}

	void GenericRendererNode::setAttached(Instance* attached_instance, const Location &relative_location, const Point &relative_point) {
		m_instance = attached_instance;
		m_location = relative_location;
		m_point = relative_point;
	}
	void GenericRendererNode::setAttached(Instance* attached_instance, const Location &relative_location) {
		m_instance = attached_instance;
		m_location = relative_location;
	}
	void GenericRendererNode::setAttached(Instance* attached_instance, const Point &relative_point) {
		m_instance = attached_instance;
		m_point = relative_point;
	}
	void GenericRendererNode::setAttached(Instance* attached_instance) {
		m_instance = attached_instance;
	}
	void GenericRendererNode::setAttached(const Location &attached_location, const Point &relative_point) {
		m_instance = NULL;
		m_location = attached_location;
		m_point = relative_point;
	}
	void GenericRendererNode::setAttached(const Location &attached_location) {
		m_instance = NULL;
		m_location = attached_location;
	}
	void GenericRendererNode::setAttached(Layer* attached_layer) {
		m_layer = attached_layer;
	}
	void GenericRendererNode::setAttached(const Point &attached_point) {
		m_instance = NULL;
		m_location = NULL;
		m_point = attached_point;
	}

	void GenericRendererNode::setRelative(const Location &relative_location) {
		if(m_instance == NULL) {
			throw NotSupported("No instance attached.");
		}
		m_location = relative_location;
	}
	void GenericRendererNode::setRelative(const Location &relative_location, Point relative_point) {
		if(m_instance == NULL) {
			throw NotSupported("No instance attached.");
		}
		m_location = relative_location;
		m_point = relative_point;
	}
	void GenericRendererNode::setRelative(const Point &relative_point) {
		if(m_instance == NULL || m_location == NULL) {
			throw NotSupported("No instance or location attached.");
		}
		m_point = relative_point;
	}

	Instance* GenericRendererNode::getAttachedInstance() {
		if(m_instance == NULL) {
			throw NotSupported("No instance attached.");
		}
		return m_instance;
	}
	Location GenericRendererNode::getAttachedLocation() {
		if(m_instance != NULL || m_location == NULL) {
			throw NotSupported("No location attached.");
		}
		return m_location;
	}
	Layer* GenericRendererNode::getAttachedLayer() {
		if(m_layer == NULL) {
			throw NotSupported("No layer attached.");
		}
		return m_layer;
	}
	Point GenericRendererNode::getAttachedPoint() {
		if(m_instance != NULL || m_location != NULL) {
			throw NotSupported("No point attached.");
		}
		return m_point;
	}

	Location GenericRendererNode::getOffsetLocation() {
		if(m_instance == NULL || m_location == NULL) {
			throw NotSupported("No location as offset used.");
		}
		return m_location;
	}
	Point GenericRendererNode::getOffsetPoint() {
		if(m_instance == NULL && m_location == NULL) {
			throw NotSupported("No point as offset used.");
		}
		return m_point;
	}

	Instance* GenericRendererNode::getInstance() {
		return m_instance;
	}
	Location GenericRendererNode::getLocation() {
		return m_location;
	}
	Layer* GenericRendererNode::getLayer() {
		return m_layer;
	}
	Point GenericRendererNode::getPoint() {
		return m_point;
	}

	Point GenericRendererNode::getCalculatedPoint(Camera* cam, Layer* layer) {
		ScreenPoint p;
		if(m_instance != NULL) {
			if(m_layer == NULL) {
				m_layer = m_instance->getLocation().getLayer();
			}
			if(m_location != NULL) {
				p = cam->toScreenCoordinates(m_instance->getLocationRef().getMapCoordinates() + m_location.getMapCoordinates());
			} else {
				p = cam->toScreenCoordinates(m_instance->getLocation().getMapCoordinates());
			}
		} else if(m_location != NULL) {
			if(m_layer == NULL) {
				m_layer = m_location.getLayer();
			}
			p = cam->toScreenCoordinates(m_location.getMapCoordinates());
		} else if(m_layer == NULL) {
			const std::list<Layer*>& layers = cam->getRenderer("GenericRenderer")->getActiveLayers();
			std::list<Layer*>::const_reverse_iterator layer_it = layers.rbegin();
			setAttached(*layer_it);
		}
		return Point(m_point.x + p.x, m_point.y + p.y);
	}

	GenericRendererLineInfo::GenericRendererLineInfo(GenericRendererNode n1, GenericRendererNode n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a):
		GenericRendererElementInfo(),
		m_edge1(n1),
		m_edge2(n2),
		m_red(r),
		m_green(g),
		m_blue(b),
		m_alpha(a) {
	}
	void GenericRendererLineInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) {
		Point p1 = m_edge1.getCalculatedPoint(cam, layer);
		Point p2 = m_edge2.getCalculatedPoint(cam, layer);
		if(m_edge1.getLayer() == layer) {
			renderbackend->drawLine(p1, p2, m_red, m_green, m_blue, m_alpha);
		}
	}

	GenericRendererPointInfo::GenericRendererPointInfo(GenericRendererNode anchor, uint8_t r, uint8_t g, uint8_t b, uint8_t a):
		GenericRendererElementInfo(),
		m_anchor(anchor),
		m_red(r),
		m_green(g),
		m_blue(b),
		m_alpha(a) {
	}
	void GenericRendererPointInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) {
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
			renderbackend->putPixel(p.x, p.y, m_red, m_green, m_blue, m_alpha);
		}
	}

	GenericRendererTriangleInfo::GenericRendererTriangleInfo(GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a):
		GenericRendererElementInfo(),
		m_edge1(n1),
		m_edge2(n2),
		m_edge3(n3),
		m_red(r),
		m_green(g),
		m_blue(b),
		m_alpha(a) {
	}
	void GenericRendererTriangleInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) {
		Point p1 = m_edge1.getCalculatedPoint(cam, layer);
		Point p2 = m_edge2.getCalculatedPoint(cam, layer);
		Point p3 = m_edge3.getCalculatedPoint(cam, layer);
		if(m_edge1.getLayer() == layer) {
			renderbackend->drawTriangle(p1, p2, p3, m_red, m_green, m_blue, m_alpha);
		}
	}

	GenericRendererQuadInfo::GenericRendererQuadInfo(GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, GenericRendererNode n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a):
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
	void GenericRendererQuadInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) {
		Point p1 = m_edge1.getCalculatedPoint(cam, layer);
		Point p2 = m_edge2.getCalculatedPoint(cam, layer);
		Point p3 = m_edge3.getCalculatedPoint(cam, layer);
		Point p4 = m_edge4.getCalculatedPoint(cam, layer);
		if(m_edge1.getLayer() == layer) {
			renderbackend->drawQuad(p1, p2, p3, p4, m_red, m_green, m_blue, m_alpha);
		}
	}

	GenericRendererVertexInfo::GenericRendererVertexInfo(GenericRendererNode center, int size, uint8_t r, uint8_t g, uint8_t b, uint8_t a):
		GenericRendererElementInfo(),
		m_center(center),
		m_size(size),
		m_red(r),
		m_green(g),
		m_blue(b),
		m_alpha(a) {
	}
	void GenericRendererVertexInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) {
		Point p = m_center.getCalculatedPoint(cam, layer);
		if(m_center.getLayer() == layer) {
			renderbackend->drawVertex(p, m_size, m_red, m_green, m_blue, m_alpha);
		}
	}

	GenericRendererImageInfo::GenericRendererImageInfo(GenericRendererNode anchor, int image):
		GenericRendererElementInfo(),
		m_anchor(anchor),
		m_image(image) {
	}
	void GenericRendererImageInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) {
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
			Image* img = &imagepool->getImage(m_image);
			Rect r;
			Rect viewport = cam->getViewPort();
			unsigned int widtht = round(img->getWidth() * cam->getZoom());
			unsigned int height = round(img->getHeight() * cam->getZoom());
			r.x = p.x-widtht/2;
			r.y = p.y-height/2;
			r.w = widtht;
			r.h = height;
			if(r.intersects(viewport))
				img->render(r);
		}
	}

	GenericRendererAnimationInfo::GenericRendererAnimationInfo(GenericRendererNode anchor, int animation):
		GenericRendererElementInfo(),
		m_anchor(anchor),
		m_animation(animation),
		m_start_time(TimeManager::instance()->getTime()),
		m_time_scale(1.0) {
	}
	void GenericRendererAnimationInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) {
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
			Animation& animation = animpool->getAnimation(m_animation);
			int animtime = scaleTime(m_time_scale, TimeManager::instance()->getTime() - m_start_time) % animation.getDuration();
			Image* img = animation.getFrameByTimestamp(animtime);
			Rect r;
			Rect viewport = cam->getViewPort();
			unsigned int widtht = round(img->getWidth() * cam->getZoom());
			unsigned int height = round(img->getHeight() * cam->getZoom());
			r.x = p.x-widtht/2;
			r.y = p.y-height/2;
			r.w = widtht;
			r.h = height;
			if(r.intersects(viewport))
				img->render(r);
		}
	}

	GenericRendererTextInfo::GenericRendererTextInfo(GenericRendererNode anchor, AbstractFont* font, std::string text):
		GenericRendererElementInfo(),
		m_anchor(anchor),
		m_font(font),
		m_text(text) {
	}
	void GenericRendererTextInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) {
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
			Image* img = m_font->getAsImageMultiline(m_text);
			Rect r;
			Rect viewport = cam->getViewPort();
			r.x = p.x-img->getWidth()/2;
			r.y = p.y-img->getHeight()/2;
			r.w = img->getWidth();
			r.h = img->getHeight();
			if(r.intersects(viewport)) {
				renderbackend->disableLighting();
				img->render(r);
				renderbackend->enableLighting();
			}
		}
	}

	GenericRendererResizeInfo::GenericRendererResizeInfo(GenericRendererNode anchor, int image, int width, int height):
		GenericRendererElementInfo(),
		m_anchor(anchor),
		m_image(image),
		m_width(width),
		m_height(height){
	}
	void GenericRendererResizeInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) {
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
			Image* img = &imagepool->getImage(m_image);
			Rect r;
			Rect viewport = cam->getViewPort();
			unsigned int widtht = round(m_width * cam->getZoom());
			unsigned int height = round(m_height * cam->getZoom());
			r.x = p.x-widtht/2;
			r.y = p.y-height/2;
			r.w = widtht;
			r.h = height;
			if(r.intersects(viewport)) {
				img->render(r);
			}
		}
	}
	
	GenericRenderer* GenericRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<GenericRenderer*>(cnt->getRenderer("GenericRenderer"));
	}

	GenericRenderer::GenericRenderer(RenderBackend* renderbackend, int position, ImagePool* imagepool, AnimationPool* animpool):
		RendererBase(renderbackend, position),
		m_imagepool(imagepool),
		m_animationpool(animpool),
		m_groups() {
		setEnabled(false);
	}

	GenericRenderer::GenericRenderer(const GenericRenderer& old):
		RendererBase(old),
		m_imagepool(old.m_imagepool),
		m_animationpool(old.m_animationpool),
		m_groups() {
		setEnabled(false);
	}

	RendererBase* GenericRenderer::clone() {
		return new GenericRenderer(*this);
	}

	GenericRenderer::~GenericRenderer() {
	}
	void GenericRenderer::addLine(const std::string &group, GenericRendererNode n1, GenericRendererNode n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		GenericRendererElementInfo* info = new GenericRendererLineInfo(n1, n2, r, g, b, a);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addPoint(const std::string &group, GenericRendererNode n, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		GenericRendererElementInfo* info = new GenericRendererPointInfo(n, r, g, b, a);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addTriangle(const std::string &group, GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		GenericRendererElementInfo* info = new GenericRendererTriangleInfo(n1, n2, n3, r, g, b, a);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addQuad(const std::string &group, GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, GenericRendererNode n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		GenericRendererElementInfo* info = new GenericRendererQuadInfo(n1, n2, n3, n4, r, g, b, a);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addVertex(const std::string &group, GenericRendererNode n, int size, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		GenericRendererElementInfo* info = new GenericRendererVertexInfo(n, size, r, g, b, a);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addText(const std::string &group, GenericRendererNode n, AbstractFont* font, const std::string &text) {
		GenericRendererElementInfo* info = new GenericRendererTextInfo(n, font, text);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addImage(const std::string &group, GenericRendererNode n, int image) {
		GenericRendererElementInfo* info = new GenericRendererImageInfo(n, image);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::addAnimation(const std::string &group, GenericRendererNode n, int animation) {
		GenericRendererElementInfo* info = new GenericRendererAnimationInfo(n, animation);
		m_groups[group].push_back(info);
	}
	void GenericRenderer::resizeImage(const std::string &group, GenericRendererNode n, int image, int width, int height) {
		GenericRendererElementInfo* info = new GenericRendererResizeInfo(n, image, width, height);
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

	void GenericRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		std::map<std::string, std::vector<GenericRendererElementInfo*> >::iterator group_it = m_groups.begin();
		for(; group_it != m_groups.end(); ++group_it) {
			std::vector<GenericRendererElementInfo*>::const_iterator info_it = group_it->second.begin();
			for (;info_it != group_it->second.end(); ++info_it) {
				(*info_it)->render(cam, layer, instances, m_renderbackend, m_imagepool, m_animationpool);
			}
		}
	}
}
