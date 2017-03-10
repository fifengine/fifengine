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

#ifndef FIFE_RENDERERNODE_H
#define FIFE_RENDERERNODE_H

// Standard C++ library includes
//#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	class InstanceDeleteListener;

	class RendererNode {
	public:
		RendererNode(Instance* attached_instance, const Location &relative_location, Layer* relative_layer, const Point &relative_point = Point(0,0));
		RendererNode(Instance* attached_instance, const Location &relative_location, const Point &relative_point = Point(0,0));
		RendererNode(Instance* attached_instance, Layer* relative_layer, const Point &relative_point = Point(0,0));
		RendererNode(Instance* attached_instance, const Point &relative_point = Point(0,0));
		RendererNode(const Location &attached_location, Layer* relative_layer, const Point &relative_point = Point(0,0));
		RendererNode(const Location &attached_location, const Point &relative_point = Point(0,0));
		RendererNode(Layer* attached_layer, const Point &relative_point = Point(0,0));
		RendererNode(const Point &attached_point);
		RendererNode(const RendererNode &old);
		RendererNode& operator=(const RendererNode &source);
		~RendererNode();

		void setAttached(Instance* attached_instance, const Location &relative_location, const Point &relative_point);
		void setAttached(Instance* attached_instance, const Location &relative_location);
		void setAttached(Instance* attached_instance, const Point &relative_point);
		void setAttached(Instance* attached_instance);
		void setAttached(const Location &attached_location, const Point &relative_point);
		void setAttached(const Location &attached_location);
		void setAttached(Layer* attached_layer);
		void setAttached(const Point &attached_point);

		void setRelative(const Location &relative_location);
		void setRelative(const Location &relative_location, Point relative_point);
		void setRelative(const Point &relative_point);

		Instance* getAttachedInstance();
		Location getAttachedLocation();
		Layer* getAttachedLayer();
		Point getAttachedPoint();

		Location getOffsetLocation();
		Point getOffsetPoint();

		Instance* getInstance();
		Location getLocation();
		const Location& getLocationRef();
		Layer* getLayer();
		Point getPoint();
		const Point& getPointRef();

		void addInstance(Instance* instance);
		void changeInstance(Instance* instance);
		void removeInstance(Instance* instance, bool listener = true);

		Point getCalculatedPoint(Camera* cam, Layer* layer, const bool zoomed = false);
	private:
		void checkDeleteListener();

		Instance* m_instance;
		Location m_location;
		Layer* m_layer;
		Point m_point;
		InstanceDeleteListener* m_listener;
	};
}

#endif
