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

%module fife
%{
#include "view/renderers/genericrenderer.h"
%}

namespace FIFE {
	class RenderBackend;
	
	class GenericRendererNode {
	public:
		GenericRendererNode(Instance* attached_instance, const Location &relative_location, Layer* relative_layer, const Point &relative_point = Point(0,0));
		GenericRendererNode(Instance* attached_instance, const Location &relative_location, const Point &relative_point = Point(0,0));
		GenericRendererNode(Instance* attached_instance, Layer* relative_layer, const Point &relative_point = Point(0,0));
		GenericRendererNode(Instance* attached_instance, const Point &relative_point = Point(0,0));
		GenericRendererNode(const Location &attached_location, Layer* relative_layer, const Point &relative_point = Point(0,0));
		GenericRendererNode(const Location &attached_location, const Point &relative_point = Point(0,0));
		GenericRendererNode(Layer* attached_layer, const Point &relative_point = Point(0,0));
		GenericRendererNode(const Point &attached_point);
		~GenericRendererNode();
		
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
		Layer* getLayer();
		Point getPoint();

		Point getCalculatedPoint(Camera* cam, Layer* layer);
	private:
		Instance* m_instance;
		Location m_location;
		Layer* m_layer;
		Point m_point;
	};
	class GenericRendererElementInfo {
	public:
		virtual ~GenericRendererElementInfo() {};
	};

	class GenericRendererLineInfo : public GenericRendererElementInfo {
	public:
		GenericRendererLineInfo(GenericRendererNode n1, GenericRendererNode n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererLineInfo() {};
	private:
		GenericRendererNode m_edge1;
		GenericRendererNode m_edge2;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};
	class GenericRendererPointInfo : public GenericRendererElementInfo {
	public:
		GenericRendererPointInfo(GenericRendererNode n, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererPointInfo() {};
	private:
		GenericRendererNode m_anchor;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};
	class GenericRendererTriangleInfo : public GenericRendererElementInfo {
	public:
		GenericRendererTriangleInfo(GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererTriangleInfo() {};
	private:
		GenericRendererNode m_edge1;
		GenericRendererNode m_edge2;
		GenericRendererNode m_edge3;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};
	class GenericRendererQuadInfo : public GenericRendererElementInfo {
	public:
		GenericRendererQuadInfo(GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, GenericRendererNode n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererQuadInfo() {};
	private:
		GenericRendererNode m_edge1;
		GenericRendererNode m_edge2;
		GenericRendererNode m_edge3;
		GenericRendererNode m_edge4;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};
	class GenericRendererVertexInfo : public GenericRendererElementInfo {
	public:
		GenericRendererVertexInfo(GenericRendererNode center, int size, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererVertexInfo() {};
	private:
		GenericRendererNode m_center;
		int m_size;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};
	class GenericRendererImageInfo : public GenericRendererElementInfo {
	public:
		GenericRendererImageInfo(GenericRendererNode n, int image);
		virtual ~GenericRendererImageInfo() {};
	private:
		GenericRendererNode m_anchor;
		int m_image;
	};
	class GenericRendererAnimationInfo : public GenericRendererElementInfo {
	public:
		GenericRendererAnimationInfo(GenericRendererNode n, int animation);
		virtual ~GenericRendererAnimationInfo() {};
	private:
		GenericRendererNode m_anchor;
		int m_animation;
	};
	class GenericRendererTextInfo : public GenericRendererElementInfo {
	public:
		GenericRendererTextInfo(GenericRendererNode n, AbstractFont* font, std::string text);
		virtual ~GenericRendererTextInfo() {};
	private:
		GenericRendererNode m_anchor;
		AbstractFont* m_font;
		std::string m_text;
	};
	class GenericRendererResizeInfo : public GenericRendererElementInfo {
	public:
		GenericRendererResizeInfo(GenericRendererNode n, int image, int width, int height);
		virtual ~GenericRendererResizeInfo() {};
	private:
		GenericRendererNode m_anchor;
		int m_image;
		int m_width;
		int m_height;
	};
	class GenericRenderer: public RendererBase {
	public:
		GenericRenderer(RenderBackend* renderbackend, int position, ImagePool* imagepool, AnimationPool* animpool);
		~GenericRenderer();
		std::string getName();
		static GenericRenderer* getInstance(IRendererContainer* cnt);
		void addLine(const std::string &group, GenericRendererNode n1, GenericRendererNode n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addPoint(const std::string &group, GenericRendererNode n, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addTriangle(const std::string &group, GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addQuad(const std::string &group, GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, GenericRendererNode n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addVertex(const std::string &group, GenericRendererNode n, int size, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addText(const std::string &group, GenericRendererNode n, AbstractFont* font, const std::string &text);
		void addImage(const std::string &group, GenericRendererNode n, int image);
		void addAnimation(const std::string &group, GenericRendererNode n, int animation);
		void resizeImage(const std::string &group, GenericRendererNode n, int image, int width, int height);
		void removeAll(const std::string &group);
	};
}
