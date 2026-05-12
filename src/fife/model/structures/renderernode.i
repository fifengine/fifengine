// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "model/structures/renderernode.h"
%}

namespace FIFE {

	%ignore RendererNode::operator=;

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

		Point getCalculatedPoint(Camera* cam, Layer* layer, const bool zoomed);
	};
}