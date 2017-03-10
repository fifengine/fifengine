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

#ifndef FIFE_OFFRENDERER_H
#define FIFE_OFFRENDERER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/animation.h"

namespace FIFE {
	class RenderBackend;
	class IFont;

	class OffRendererElementInfo {
	public:
		virtual void render(RenderBackend* renderbackend) {};
		virtual ~OffRendererElementInfo() {};
	};

	class OffRendererLineInfo : public OffRendererElementInfo {
	public:
		void render(RenderBackend* renderbackend);
		OffRendererLineInfo(Point n1, Point n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~OffRendererLineInfo() {};
	private:
		Point m_edge1;
		Point m_edge2;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};
	class OffRendererPointInfo : public OffRendererElementInfo {
	public:
		void render(RenderBackend* renderbackend);
		OffRendererPointInfo(Point n, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~OffRendererPointInfo() {};
	private:
		Point m_anchor;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};
	class OffRendererTriangleInfo : public OffRendererElementInfo {
	public:
		void render(RenderBackend* renderbackend);
		OffRendererTriangleInfo(Point n1, Point n2, Point n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~OffRendererTriangleInfo() {};
	private:
		Point m_edge1;
		Point m_edge2;
		Point m_edge3;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};
	class OffRendererQuadInfo : public OffRendererElementInfo {
	public:
		void render(RenderBackend* renderbackend);
		OffRendererQuadInfo(Point n1, Point n2, Point n3, Point n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~OffRendererQuadInfo() {};
	private:
		Point m_edge1;
		Point m_edge2;
		Point m_edge3;
		Point m_edge4;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};

	class OffRendererVertexInfo : public OffRendererElementInfo {
	public:
		void render(RenderBackend* renderbackend);
		OffRendererVertexInfo(Point center, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~OffRendererVertexInfo() {};
	private:
		Point m_center;
		int32_t m_size;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
		uint8_t m_alpha;
	};

	class OffRendererImageInfo : public OffRendererElementInfo {
	public:
		void render(RenderBackend* renderbackend);
		OffRendererImageInfo(Point n, ImagePtr image);
		virtual ~OffRendererImageInfo() {};
	private:
		Point m_anchor;
		ImagePtr m_image;
	};
	class OffRendererAnimationInfo : public OffRendererElementInfo {
	public:
		void render(RenderBackend* renderbackend);
		OffRendererAnimationInfo(Point n, AnimationPtr animation);
		virtual ~OffRendererAnimationInfo() {};
	private:
		Point m_anchor;
		AnimationPtr m_animation;
		uint32_t m_start_time;
		float m_time_scale;
	};
	class OffRendererTextInfo : public OffRendererElementInfo {
	public:
		void render(RenderBackend* renderbackend);
		OffRendererTextInfo(Point n, IFont* font, std::string text);
		virtual ~OffRendererTextInfo() {};
	private:
		Point m_anchor;
		IFont* m_font;
		std::string m_text;
	};
	class OffRendererResizeInfo : public OffRendererElementInfo {
	public:
		void render(RenderBackend* renderbackend);
		OffRendererResizeInfo(Point n, ImagePtr image, int32_t width, int32_t height);
		virtual ~OffRendererResizeInfo() {};
	private:
		Point m_anchor;
		ImagePtr m_image;
		int32_t m_width;
		int32_t m_height;
	};
	class OffRenderer {
	public:
		/** constructor.
		 * @param renderbackend to use
		 */
		OffRenderer(RenderBackend* renderbackend);

		/** Destructor.
		 */
		virtual ~OffRenderer();

		void setEnabled(bool enabled);
		bool isEnabled();
		void setClipArea(Rect area);
		const Rect& getClipArea() const;
		void render();

		void addLine(const std::string &group, Point n1, Point n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addPoint(const std::string &group, Point n, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addTriangle(const std::string &group, Point n1, Point n2, Point n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addQuad(const std::string &group, Point n1, Point n2, Point n3, Point n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addVertex(const std::string &group, Point n, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addText(const std::string &group, Point n, IFont* font, const std::string &text);
		void addImage(const std::string &group, Point n, ImagePtr image);
		void addAnimation(const std::string &group, Point n, AnimationPtr animation);
		void resizeImage(const std::string &group, Point n, ImagePtr image, int32_t width, int32_t height);
		void removeAll(const std::string &group);
		void removeAll();

	private:
		std::map<std::string, std::vector<OffRendererElementInfo*> > m_groups;
		RenderBackend* m_renderbackend;
		bool m_enabled;
		Rect m_area;
	};

}

#endif
