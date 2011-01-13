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

#ifndef FIFE_LIGHTRENDERER_H
#define FIFE_LIGHTRENDERER_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "view/rendererbase.h"

namespace FIFE {
	class RenderBackend;
	class AbstractFont;

	class LightRendererNode {
	public:
		LightRendererNode(Instance* attached_instance, const Location &relative_location, Layer* relative_layer, const Point &relative_point = Point(0,0));
		LightRendererNode(Instance* attached_instance, const Location &relative_location, const Point &relative_point = Point(0,0));
		LightRendererNode(Instance* attached_instance, Layer* relative_layer, const Point &relative_point = Point(0,0));
		LightRendererNode(Instance* attached_instance, const Point &relative_point = Point(0,0));
		LightRendererNode(const Location &attached_location, Layer* relative_layer, const Point &relative_point = Point(0,0));
		LightRendererNode(const Location &attached_location, const Point &relative_point = Point(0,0));
		LightRendererNode(Layer* attached_layer, const Point &relative_point = Point(0,0));
		LightRendererNode(const Point &attached_point);
		~LightRendererNode();

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

	class LightRendererElementInfo {
	public:
		virtual void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {};
		virtual std::string getName() { return 0; };
		virtual LightRendererNode* getNode() { return NULL; };
		virtual int32_t getId() { return -1; };
		virtual int32_t getSrcBlend() { return -1; };
		virtual int32_t getDstBlend() { return -1; };
		virtual void setStencil(uint8_t stencil_ref, float alpha_ref) {};
		virtual int32_t getStencil() { return 0; };
		virtual float getAlpha() { return 0; };
		virtual void removeStencil() {};
		virtual std::vector<uint8_t> getColor() { return std::vector<uint8_t>(); };
		virtual float getRadius() { return 0; };
		virtual int32_t getSubdivisions() { return 0; };
		virtual float getXStretch() { return 0; };
		virtual float getYStretch() { return 0; };
		virtual ~LightRendererElementInfo() {};
	};

	class LightRendererImageInfo : public LightRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend);
		std::string getName() { return "image"; };
		LightRendererNode* getNode() { return &m_anchor; };
		int32_t getId() { return m_image; };
		int32_t getSrcBlend() { return m_src; };
		int32_t getDstBlend() { return m_dst; };
		void setStencil(uint8_t stencil_ref, float alpha_ref);
		int32_t getStencil();
		float getAlpha();
		void removeStencil();
		LightRendererImageInfo(LightRendererNode n, int32_t image, int32_t src, int32_t dst);
		virtual ~LightRendererImageInfo() {};
	private:
		LightRendererNode m_anchor;
		int32_t m_image;
		int32_t m_src;
		int32_t m_dst;
		bool m_stencil;
		uint8_t m_stencil_ref;
		float m_alpha_ref;
	};
	class LightRendererAnimationInfo : public LightRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend);
		std::string getName() { return "animation"; };
		LightRendererNode* getNode() { return &m_anchor; };
		int32_t getId() { return m_animation; };
		int32_t getSrcBlend() { return m_src; };
		int32_t getDstBlend() { return m_dst; };
		void setStencil(uint8_t stencil_ref, float alpha_ref);
		int32_t getStencil();
		float getAlpha();
		void removeStencil();
		LightRendererAnimationInfo(LightRendererNode n, int32_t animation, int32_t src, int32_t dst);
		virtual ~LightRendererAnimationInfo() {};
	private:
		LightRendererNode m_anchor;
		int32_t m_animation;
		int32_t m_src;
		int32_t m_dst;
		uint32_t m_start_time;
		float m_time_scale;
		bool m_stencil;
		uint8_t m_stencil_ref;
		float m_alpha_ref;
	};
	class LightRendererSimpleLightInfo : public LightRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend);
		std::string getName() { return "simple"; };
		LightRendererNode* getNode() { return &m_anchor; };
		int32_t getSrcBlend() { return m_src; };
		int32_t getDstBlend() { return m_dst; };
		void setStencil(uint8_t stencil_ref, float alpha_ref);
		int32_t getStencil();
		float getAlpha();
		void removeStencil();
		std::vector<uint8_t> getColor();
		float getRadius() { return m_radius; };
		int32_t getSubdivisions() { return m_subdivisions; };
		float getXStretch() { return m_xstretch; };
		float getYStretch() { return m_ystretch; };
		LightRendererSimpleLightInfo(LightRendererNode n, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t r, uint8_t g, uint8_t b, int32_t src, int32_t dst);
		virtual ~LightRendererSimpleLightInfo() {};
	private:
		LightRendererNode m_anchor;
		uint8_t m_intensity;
		float m_radius;
		int32_t m_subdivisions;
		float m_xstretch;
		float m_ystretch;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		int32_t m_src;
		int32_t m_dst;
		bool m_stencil;
		uint8_t m_stencil_ref;
		float m_alpha_ref;
	};
	class LightRendererResizeInfo : public LightRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend);
		std::string getName() { return "resize"; };
		LightRendererNode* getNode() { return &m_anchor; };
		int32_t getId() { return m_image; };
		int32_t getSrcBlend() { return m_src; };
		int32_t getDstBlend() { return m_dst; };
		void setStencil(uint8_t stencil_ref, float alpha_ref);
		int32_t getStencil();
		float getAlpha();
		void removeStencil();
		LightRendererResizeInfo(LightRendererNode n, int32_t image, int32_t width, int32_t height, int32_t src, int32_t dst);
		virtual ~LightRendererResizeInfo() {};
	private:
		LightRendererNode m_anchor;
		int32_t m_image;
		int32_t m_width;
		int32_t m_height;
		int32_t m_src;
		int32_t m_dst;
		bool m_stencil;
		uint8_t m_stencil_ref;
		float m_alpha_ref;
	};
	class LightRenderer: public RendererBase {
	public:
		/** constructor.
		 * @param renderbackend to use
		 * @param position position for this renderer in rendering pipeline
		 */
		LightRenderer(RenderBackend* renderbackend, int32_t position);

		LightRenderer(const LightRenderer& old);

		RendererBase* clone();

		/** Destructor.
		 */
		virtual ~LightRenderer();
		void render(Camera* cam, Layer* layer, RenderList& instances);
		std::string getName() { return "LightRenderer"; }

		/** Gets instance for interface access
		 */
		static LightRenderer* getInstance(IRendererContainer* cnt);

		void addImage(const std::string &group, LightRendererNode n, int32_t image, int32_t src=-1, int32_t dst=-1);
		void addAnimation(const std::string &group, LightRendererNode n, int32_t animation, int32_t src=-1, int32_t dst=-1);
		void addSimpleLight(const std::string &group, LightRendererNode n, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t r, uint8_t g, uint8_t b, int32_t src=-1, int32_t dst=-1);
		void resizeImage(const std::string &group, LightRendererNode n, int32_t image, int32_t width, int32_t height, int32_t src=-1, int32_t dst=-1);
		void addStencilTest(const std::string &group, uint8_t stencil_ref=0, float alpha_ref=0.0);
		void removeStencilTest(const std::string &group);
		std::list<std::string> getGroups();
		std::vector<LightRendererElementInfo*> getLightInfo(const std::string &group);
		void removeAll(const std::string &group);
		void removeAll();
		void reset();

	private:
		std::map<std::string, std::vector<LightRendererElementInfo*> > m_groups;
	};

}

#endif
