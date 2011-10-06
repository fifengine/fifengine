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

#ifndef FIFE_GENERICRENDERER_H
#define FIFE_GENERICRENDERER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/renderernode.h"
#include "view/rendererbase.h"
#include "video/animation.h"

namespace FIFE {
	class RenderBackend;
	class IFont;

	class GenericRendererElementInfo {
	public:
		virtual void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {};
		virtual ~GenericRendererElementInfo() {};
	};

	class GenericRendererLineInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend);
		GenericRendererLineInfo(RendererNode n1, RendererNode n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererLineInfo() {};
	private:
		RendererNode m_edge1;
		RendererNode m_edge2;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};
	class GenericRendererPointInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend);
		GenericRendererPointInfo(RendererNode n, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererPointInfo() {};
	private:
		RendererNode m_anchor;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};
	class GenericRendererTriangleInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend);
		GenericRendererTriangleInfo(RendererNode n1, RendererNode n2, RendererNode n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererTriangleInfo() {};
	private:
		RendererNode m_edge1;
		RendererNode m_edge2;
		RendererNode m_edge3;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};
	class GenericRendererQuadInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend);
		GenericRendererQuadInfo(RendererNode n1, RendererNode n2, RendererNode n3, RendererNode n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererQuadInfo() {};
	private:
		RendererNode m_edge1;
		RendererNode m_edge2;
		RendererNode m_edge3;
		RendererNode m_edge4;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};

	class GenericRendererVertexInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend);
		GenericRendererVertexInfo(RendererNode center, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererVertexInfo() {};
	private:
		RendererNode m_center;
		int32_t m_size;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};

	class GenericRendererImageInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend);
		GenericRendererImageInfo(RendererNode n, ImagePtr image, bool zoomed = true);
		virtual ~GenericRendererImageInfo() {};
	private:
		RendererNode m_anchor;
		ImagePtr m_image;
		bool m_zoomed;
	};
	class GenericRendererAnimationInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend);
		GenericRendererAnimationInfo(RendererNode n, AnimationPtr animation, bool zoomed = true);
		virtual ~GenericRendererAnimationInfo() {};
	private:
		RendererNode m_anchor;
		AnimationPtr m_animation;
		uint32_t m_start_time;
		float m_time_scale;
		bool m_zoomed;
	};
	class GenericRendererTextInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend);
		GenericRendererTextInfo(RendererNode n, IFont* font, std::string text);
		virtual ~GenericRendererTextInfo() {};
	private:
		RendererNode m_anchor;
		IFont* m_font;
		std::string m_text;
	};
	class GenericRendererResizeInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend);
		GenericRendererResizeInfo(RendererNode n, ImagePtr image, int32_t width, int32_t height, bool zoomed = true);
		virtual ~GenericRendererResizeInfo() {};
	private:
		RendererNode m_anchor;
		ImagePtr m_image;
		int32_t m_width;
		int32_t m_height;
		bool m_zoomed;
	};
	class GenericRenderer: public RendererBase {
	public:
		/** constructor.
		 * @param renderbackend to use
		 * @param position position for this renderer in rendering pipeline
		 */
		GenericRenderer(RenderBackend* renderbackend, int32_t position);

		GenericRenderer(const GenericRenderer& old);

		RendererBase* clone();

		/** Destructor.
		 */
		virtual ~GenericRenderer();
		void render(Camera* cam, Layer* layer, RenderList& instances);
		std::string getName() { return "GenericRenderer"; }

		/** Gets instance for interface access
		 */
		static GenericRenderer* getInstance(IRendererContainer* cnt);

		void addLine(const std::string &group, RendererNode n1, RendererNode n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addPoint(const std::string &group, RendererNode n, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addTriangle(const std::string &group, RendererNode n1, RendererNode n2, RendererNode n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addQuad(const std::string &group, RendererNode n1, RendererNode n2, RendererNode n3, RendererNode n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addVertex(const std::string &group, RendererNode n, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addText(const std::string &group, RendererNode n, IFont* font, const std::string &text);
		void addImage(const std::string &group, RendererNode n, ImagePtr image, bool zoomed = true);
		void addAnimation(const std::string &group, RendererNode n, AnimationPtr animation, bool zoomed = true);
		void resizeImage(const std::string &group, RendererNode n, ImagePtr image, int32_t width, int32_t height, bool zoomed = true);
		void removeAll(const std::string &group);
		void removeAll();
		void reset();

	private:
		std::map<std::string, std::vector<GenericRendererElementInfo*> > m_groups;
	};

}

#endif
