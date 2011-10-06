/***************************************************************************
 *   Copyright (C) 2005-2011 by the FIFE team                              *
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
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "view/camera.h"

#include "renderernode.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);

	RendererNode::RendererNode(Instance* attached_instance, const Location &relative_location, Layer* relative_layer, const Point &relative_point):
		m_instance(attached_instance),
		m_location(relative_location),
		m_layer(relative_layer),
		m_point(relative_point) {
	}
	RendererNode::RendererNode(Instance* attached_instance, const Location &relative_location, const Point &relative_point):
		m_instance(attached_instance),
		m_location(relative_location),
		m_layer(NULL),
		m_point(relative_point) {
	}
	RendererNode::RendererNode(Instance* attached_instance, Layer* relative_layer, const Point &relative_point):
		m_instance(attached_instance),
		m_location(NULL),
		m_layer(relative_layer),
		m_point(relative_point) {
	}
	RendererNode::RendererNode(Instance* attached_instance, const Point &relative_point):
		m_instance(attached_instance),
		m_location(NULL),
		m_layer(NULL),
		m_point(relative_point) {
	}
	RendererNode::RendererNode(const Location &attached_location, Layer* relative_layer, const Point &relative_point):
		m_instance(NULL),
		m_location(attached_location),
		m_layer(relative_layer),
		m_point(relative_point) {
	}
	RendererNode::RendererNode(const Location &attached_location, const Point &relative_point):
		m_instance(NULL),
		m_location(attached_location),
		m_layer(NULL),
		m_point(relative_point) {
	}
	RendererNode::RendererNode(Layer* attached_layer, const Point &relative_point):
		m_instance(NULL),
		m_location(NULL),
		m_layer(attached_layer),
		m_point(relative_point) {
	}
	RendererNode::RendererNode(const Point &attached_point):
		m_instance(NULL),
		m_location(NULL),
		m_layer(NULL),
		m_point(attached_point) {
	}
	RendererNode::~RendererNode() {
	}

	void RendererNode::setAttached(Instance* attached_instance, const Location &relative_location, const Point &relative_point) {
		m_instance = attached_instance;
		m_location = relative_location;
		m_point = relative_point;
	}
	void RendererNode::setAttached(Instance* attached_instance, const Location &relative_location) {
		m_instance = attached_instance;
		m_location = relative_location;
	}
	void RendererNode::setAttached(Instance* attached_instance, const Point &relative_point) {
		m_instance = attached_instance;
		m_point = relative_point;
	}
	void RendererNode::setAttached(Instance* attached_instance) {
		m_instance = attached_instance;
	}
	void RendererNode::setAttached(const Location &attached_location, const Point &relative_point) {
		m_instance = NULL;
		m_location = attached_location;
		m_point = relative_point;
	}
	void RendererNode::setAttached(const Location &attached_location) {
		m_instance = NULL;
		m_location = attached_location;
	}
	void RendererNode::setAttached(Layer* attached_layer) {
		m_layer = attached_layer;
	}
	void RendererNode::setAttached(const Point &attached_point) {
		m_instance = NULL;
		m_location = NULL;
		m_point = attached_point;
	}

	void RendererNode::setRelative(const Location &relative_location) {
		if(m_instance == NULL) {
			FL_WARN(_log, LMsg("RendererNode::setRelative(Location) - ") << "No instance attached.");
		}
		m_location = relative_location;
	}
	void RendererNode::setRelative(const Location &relative_location, Point relative_point) {
		if(m_instance == NULL) {
			FL_WARN(_log, LMsg("RendererNode::setRelative(Location, Point) - ") << "No instance attached.");
		}
		m_location = relative_location;
		m_point = relative_point;
	}
	void RendererNode::setRelative(const Point &relative_point) {
		if(m_instance == NULL || m_location == NULL) {
			FL_WARN(_log, LMsg("RendererNode::setRelative(Point) - ") << "No instance or location attached.");
		}
		m_point = relative_point;
	}

	Instance* RendererNode::getAttachedInstance() {
		if(m_instance == NULL) {
			FL_WARN(_log, LMsg("RendererNode::getAttachedInstance() - ") << "No instance attached.");
		}
		return m_instance;
	}
	Location RendererNode::getAttachedLocation() {
		if(m_instance != NULL || m_location == NULL) {
			FL_WARN(_log, LMsg("RendererNode::getAttachedLocation() - ") << "No location attached.");
		}
		return m_location;
	}
	Layer* RendererNode::getAttachedLayer() {
		if(m_layer == NULL) {
			FL_WARN(_log, LMsg("RendererNode::getAttachedLayer() - ") << "No layer attached.");
		}
		return m_layer;
	}
	Point RendererNode::getAttachedPoint() {
		if(m_instance != NULL || m_location != NULL) {
			FL_WARN(_log, LMsg("RendererNode::getAttachedPoint() - ") << "No point attached.");
		}
		return m_point;
	}

	Location RendererNode::getOffsetLocation() {
		if(m_instance == NULL || m_location == NULL) {
			FL_WARN(_log, LMsg("RendererNode::getOffsetLocation() - ") << "No location as offset used.");
		}
		return m_location;
	}
	Point RendererNode::getOffsetPoint() {
		if(m_instance == NULL && m_location == NULL) {
			FL_WARN(_log, LMsg("RendererNode::getOffsetPoint() - ") << "No point as offset used.");
		}
		return m_point;
	}

	Instance* RendererNode::getInstance() {
		return m_instance;
	}
	Location RendererNode::getLocation() {
		return m_location;
	}
	Layer* RendererNode::getLayer() {
		return m_layer;
	}
	Point RendererNode::getPoint() {
		return m_point;
	}

	Point RendererNode::getCalculatedPoint(Camera* cam, Layer* layer) {
		ScreenPoint p;
		if(m_instance != NULL) {
			if(m_layer == NULL) {
				m_layer = m_instance->getLocationRef().getLayer();
			}
			if(m_location != NULL) {
				p = cam->toScreenCoordinates(m_instance->getLocationRef().getMapCoordinates() + m_location.getMapCoordinates());
			} else {
				p = cam->toScreenCoordinates(m_instance->getLocationRef().getMapCoordinates());
			}
		} else if(m_location != NULL) {
			if(m_layer == NULL) {
				m_layer = m_location.getLayer();
			}
			p = cam->toScreenCoordinates(m_location.getMapCoordinates());
		} else if(m_layer == NULL) {
			// FIXME
			FL_WARN(_log, LMsg("RendererNode::getCalculatedPoint(Camera, Layer) - ") << "No layer attached. So we use the first active layer of the renderer.");
			setAttached(layer);
		}
		return Point(m_point.x + p.x, m_point.y + p.y);
	}
}
