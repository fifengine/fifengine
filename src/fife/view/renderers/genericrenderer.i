// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/renderers/genericrenderer.h"
%}

namespace FIFE {
	class RenderBackend;
	
	class GenericRendererElementInfo {
	public:
		virtual ~GenericRendererElementInfo() {};
	};

	class GenericRendererLineInfo : public GenericRendererElementInfo {
	public:
		GenericRendererLineInfo(RendererNode n1, RendererNode n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererLineInfo() {};
	};
	class GenericRendererPointInfo : public GenericRendererElementInfo {
	public:
		GenericRendererPointInfo(RendererNode n, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererPointInfo() {};
	};
	class GenericRendererTriangleInfo : public GenericRendererElementInfo {
	public:
		GenericRendererTriangleInfo(RendererNode n1, RendererNode n2, RendererNode n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererTriangleInfo() {};
	};
	class GenericRendererQuadInfo : public GenericRendererElementInfo {
	public:
		GenericRendererQuadInfo(RendererNode n1, RendererNode n2, RendererNode n3, RendererNode n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererQuadInfo() {};
	};
	class GenericRendererVertexInfo : public GenericRendererElementInfo {
	public:
		GenericRendererVertexInfo(RendererNode center, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual ~GenericRendererVertexInfo() {};
	};
	class GenericRendererImageInfo : public GenericRendererElementInfo {
	public:
		GenericRendererImageInfo(RendererNode n, ImagePtr image, bool zoomed = true);
		virtual ~GenericRendererImageInfo() {};
	};
	class GenericRendererAnimationInfo : public GenericRendererElementInfo {
	public:
		GenericRendererAnimationInfo(RendererNode n, AnimationPtr animation, bool zoomed = true);
		virtual ~GenericRendererAnimationInfo() {};
	};
	class GenericRendererTextInfo : public GenericRendererElementInfo {
	public:
		GenericRendererTextInfo(RendererNode n, IFont* font, std::string text, bool zoomed = true);
		virtual ~GenericRendererTextInfo() {};
	};
	class GenericRendererResizeInfo : public GenericRendererElementInfo {
	public:
		GenericRendererResizeInfo(RendererNode n, ImagePtr image, int32_t width, int32_t height, bool zoomed = true);
		virtual ~GenericRendererResizeInfo() {};
	};
	class GenericRenderer: public RendererBase {
	public:
		GenericRenderer(RenderBackend* renderbackend, int32_t position);
		~GenericRenderer();
		std::string getName();
		static GenericRenderer* getInstance(IRendererContainer* cnt);
		void addLine(const std::string &group, RendererNode n1, RendererNode n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addPoint(const std::string &group, RendererNode n, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addTriangle(const std::string &group, RendererNode n1, RendererNode n2, RendererNode n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addQuad(const std::string &group, RendererNode n1, RendererNode n2, RendererNode n3, RendererNode n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addVertex(const std::string &group, RendererNode n, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void addText(const std::string &group, RendererNode n, IFont* font, const std::string &text, bool zoomed = true);
		void addImage(const std::string &group, RendererNode n, ImagePtr image, bool zoomed = true);
		void addAnimation(const std::string &group, RendererNode n, AnimationPtr animation, bool zoomed = true);
		void resizeImage(const std::string &group, RendererNode n, ImagePtr image, int32_t width, int32_t height, bool zoomed = true);
		void removeAll(const std::string &group);
		void removeAll();
	};
}