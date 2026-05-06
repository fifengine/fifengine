// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/renderers/offrenderer.h"
%}

namespace FIFE {
	class RenderBackend;
	
	class OffRendererElementInfo {
	public:
		virtual ~OffRendererElementInfo() {};
	};

	class OffRendererLineInfo : public OffRendererElementInfo {
	public:
		OffRendererLineInfo(Point n1, Point n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~OffRendererLineInfo() {};
	};
	class OffRendererPointInfo : public OffRendererElementInfo {
	public:
		OffRendererPointInfo(Point n, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~OffRendererPointInfo() {};
	};
	class OffRendererTriangleInfo : public OffRendererElementInfo {
	public:
		OffRendererTriangleInfo(Point n1, Point n2, Point n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~OffRendererTriangleInfo() {};
	};
	class OffRendererQuadInfo : public OffRendererElementInfo {
	public:
		OffRendererQuadInfo(Point n1, Point n2, Point n3, Point n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~OffRendererQuadInfo() {};
	};
	class OffRendererVertexInfo : public OffRendererElementInfo {
	public:
		OffRendererVertexInfo(Point center, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~OffRendererVertexInfo() {};
	};
	class OffRendererImageInfo : public OffRendererElementInfo {
	public:
		OffRendererImageInfo(Point n, ImagePtr image);
		virtual ~OffRendererImageInfo() {};
	};
	class OffRendererAnimationInfo : public OffRendererElementInfo {
	public:
		OffRendererAnimationInfo(Point n, AnimationPtr animation);
		virtual ~OffRendererAnimationInfo() {};
	};
	class OffRendererTextInfo : public OffRendererElementInfo {
	public:
		OffRendererTextInfo(Point n, IFont* font, std::string text);
		virtual ~OffRendererTextInfo() {};
	};
	class OffRendererResizeInfo : public OffRendererElementInfo {
	public:
		OffRendererResizeInfo(Point n, ImagePtr image, int32_t width, int32_t height);
		virtual ~OffRendererResizeInfo() {};
	};
	class OffRenderer {
	public:
		OffRenderer(RenderBackend* renderbackend);
		~OffRenderer();
		void setEnabled(bool enabled);
		bool isEnabled();
		void setClipArea(Rect area);
		const Rect& getClipArea() const;
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
	};
}