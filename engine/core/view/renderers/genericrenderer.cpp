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
#include "genericrenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);

	GenericRendererNode::GenericRendererNode(Instance* attached_instance, Location* relative_location, const Point &relative_point):
		m_instance(attached_instance),
		m_location(relative_location),
		m_point(relative_point) {
	}
	GenericRendererNode::GenericRendererNode(Instance* attached_instance, const Point &relative_point):
		m_instance(attached_instance),
		m_location(NULL),
		m_point(relative_point) {
	}
	GenericRendererNode::GenericRendererNode(Location* attached_location, const Point &relative_point):
		m_instance(NULL),
		m_location(attached_location),
		m_point(relative_point) {
	}
	GenericRendererNode::GenericRendererNode(const Point &attached_point):
		m_instance(NULL),
		m_location(NULL),
		m_point(attached_point) {
	}
	GenericRendererNode::~GenericRendererNode() {
	}

	void GenericRendererNode::setAttached(Instance* attached_instance, const Location &relative_location, const Point &relative_point) {
	}
	void GenericRendererNode::setAttached(Instance* attached_instance, const Location &relative_location) {
	}
	void GenericRendererNode::setAttached(Instance* attached_instance, const Point &relative_point) {
	}
	void GenericRendererNode::setAttached(Instance* attached_instance) {
	}
	void GenericRendererNode::setAttached(const Location &attached_location, const Point &relative_point) {
	}
	void GenericRendererNode::setAttached(const Location &attached_location) {
	}
	void GenericRendererNode::setAttached(const Point &attached_point) {
	}

	void GenericRendererNode::setRelative(const Location &relative_location) {
	}
	void GenericRendererNode::setRelative(const Location &relative_location, Point relative_point) {
	}
	void GenericRendererNode::setRelative(const Point &relative_point) {
	}

	Instance* GenericRendererNode::getAttachedInstance() {
		if(m_instance == NULL) {
			throw NotSupported("No instance attached.");
		}
		return m_instance;
	}
	Location* GenericRendererNode::getAttachedLocation() {
		if(m_instance != NULL or m_location == NULL) {
			throw NotSupported("No location attached.");
		}
		return m_location;
	}
	Point GenericRendererNode::getAttachedPoint() {
		if(m_instance != NULL or m_location != NULL) {
			throw NotSupported("No point attached.");
		}
		return m_point;
	}

	Location* GenericRendererNode::getOffsetLocation() {
		if(m_instance == NULL or m_location == NULL) {
			throw NotSupported("No location as offset used.");
		}
		return m_location;
	}
	Point GenericRendererNode::getOffsetPoint() {
		if(m_instance == NULL and m_location == NULL) {
			throw NotSupported("No point as offset used.");
		}
		return m_point;
	}

	Instance* GenericRendererNode::getInstance() {
		return m_instance;
	}
	Location* GenericRendererNode::getLocation() {
		return m_location;
	}
	Point GenericRendererNode::getPoint() {
		return m_point;
	}

	Point GenericRendererNode::getCalculatedPoint(Camera* cam, Layer* layer, std::vector<Instance*>& instances) {
		ScreenPoint p;
		if(m_instance != NULL) {
			if(m_location != NULL) {
				p = cam->toScreenCoordinates(m_instance->getLocationRef().getMapCoordinates() + m_location->getMapCoordinates());
			}
			else {
				p = cam->toScreenCoordinates(m_instance->getLocation().getMapCoordinates());
			}
		}
		else if(m_location != NULL) {
			p = cam->toScreenCoordinates(m_location->getMapCoordinates());
		}
		else
			return m_point;
		return Point(m_point.x + p.x, m_point.y + p.y);
	}

	GenericRendererLineInfo::GenericRendererLineInfo(GenericRendererNode n1, GenericRendererNode n2, uint8_t r, uint8_t g, uint8_t b):
		GenericRendererElementInfo(),
		n1(n1),
		n2(n2),
		r(r),
		g(g),
		b(b) {
	}
	void GenericRendererLineInfo::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend) {
		Point p1 = n1.getCalculatedPoint(cam, layer, instances);
		Point p2 = n2.getCalculatedPoint(cam, layer, instances);
		renderbackend->drawLine(p1, p2, r, g, b);
	}
	
	GenericRendererPointInfo::GenericRendererPointInfo(GenericRendererNode n, uint8_t r, uint8_t g, uint8_t b):
		GenericRendererElementInfo(),
		n(n),
		r(r),
		g(g),
		b(b) {
	}
	void GenericRendererPointInfo::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend) {
		Point p = n.getCalculatedPoint(cam, layer, instances);
		renderbackend->putPixel(p.x, p.y, r, g, b);
	}
	
	GenericRendererQuadInfo::GenericRendererQuadInfo(GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, GenericRendererNode n4, uint8_t r, uint8_t g, uint8_t b):
		GenericRendererElementInfo(),
		n1(n1),
		n2(n2),
		n3(n3),
		n4(n4),
		r(r),
		g(g),
		b(b) {
	}
	void GenericRendererQuadInfo::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend) {
		Point p1 = n1.getCalculatedPoint(cam, layer, instances);
		Point p2 = n2.getCalculatedPoint(cam, layer, instances);
		Point p3 = n3.getCalculatedPoint(cam, layer, instances);
		Point p4 = n4.getCalculatedPoint(cam, layer, instances);
		renderbackend->drawQuad(p1, p2, p3, p4, r, g, b);
	}
	
	GenericRendererImageInfo::GenericRendererImageInfo(GenericRendererNode n, int image):
		GenericRendererElementInfo(),
		n(n),
		image(image) {
	}
	void GenericRendererImageInfo::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend) {
		return;
	}
	
	GenericRendererAnimationInfo::GenericRendererAnimationInfo(GenericRendererNode n, int animation):
		GenericRendererElementInfo(),
		n(n),
		animation(animation) {
	}
	void GenericRendererAnimationInfo::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend) {
		return;
	}
	
	GenericRendererTextInfo::GenericRendererTextInfo(GenericRendererNode n, AbstractFont* font, std::string text):
		GenericRendererElementInfo(),
		n(n),
		font(font),
		text(text) {
	}
	void GenericRendererTextInfo::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend) {
		return;
	}
	
	GenericRenderer* GenericRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<GenericRenderer*>(cnt->getRenderer("GenericRenderer"));
	}
	
	GenericRenderer::GenericRenderer(RenderBackend* renderbackend, int position):
		RendererBase(renderbackend, position),
		m_infos() {
		setEnabled(false);
	}

	GenericRenderer::GenericRenderer(const GenericRenderer& old):
		RendererBase(old) {
		setEnabled(false);
	}

	RendererBase* GenericRenderer::clone() {
		return new GenericRenderer(*this);
	}

	GenericRenderer::~GenericRenderer() {
	}
	void GenericRenderer::addLine(GenericRendererNode n1, GenericRendererNode n2, uint8_t r, uint8_t g, uint8_t b) {
		GenericRendererElementInfo* info = new GenericRendererLineInfo(n1, n2, r, g, b);
		m_infos.push_back(info);
	}
	void GenericRenderer::addPoint(GenericRendererNode n, uint8_t r, uint8_t g, uint8_t b) {
		GenericRendererElementInfo* info = new GenericRendererPointInfo(n, r, g, b);
		m_infos.push_back(info);
	}
	void GenericRenderer::addQuad(GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, GenericRendererNode n4, uint8_t r, uint8_t g, uint8_t b) {
		GenericRendererElementInfo* info = new GenericRendererQuadInfo(n1, n2, n3, n4, r, g, b);
		m_infos.push_back(info);
	}
	void GenericRenderer::addText(GenericRendererNode n, AbstractFont* font, std::string text) {
		GenericRendererElementInfo* info = new GenericRendererTextInfo(n, font, text);
		m_infos.push_back(info);
	}
	void GenericRenderer::addImage(GenericRendererNode n, int image) {
		GenericRendererElementInfo* info = new GenericRendererImageInfo(n, image);
		m_infos.push_back(info);
	}
	void GenericRenderer::addAnimation(GenericRendererNode n, int animation) {
		GenericRendererElementInfo* info = new GenericRendererAnimationInfo(n, animation);
		m_infos.push_back(info);
	}
	void GenericRenderer::removeAll() {
		std::vector<GenericRendererElementInfo*>::const_iterator info_it = m_infos.begin();
		for (;info_it != m_infos.end(); ++info_it) {
			delete *info_it;
		}
		m_infos.clear();
	}

	void GenericRenderer::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances) {
		std::vector<GenericRendererElementInfo*>::const_iterator info_it = m_infos.begin();
		for (;info_it != m_infos.end(); ++info_it) {
			GenericRendererElementInfo* info = *info_it;
			info->render(cam, layer, instances, m_renderbackend);
		}
	}
}
