// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/renderers/targetrenderer.h"
%}

namespace FIFE {

	class RenderTarget {
	public:
		~RenderTarget();

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
		void render();
		ImagePtr getTarget();		
	private:
		// Non copyable
		RenderTarget(const RenderTarget& rhs); /* = delete */
		RenderTarget& operator=(const RenderTarget& rhs); /* = delete */
	};
	typedef SharedPtr<RenderTarget> RenderTargetPtr;
	%template(SharedRenderTargetPointer) SharedPtr<RenderTarget>;

	class TargetRenderer {
	public:
		TargetRenderer(RenderBackend* renderbackend);
		virtual ~TargetRenderer();

		RenderTargetPtr createRenderTarget(const std::string& name, uint32_t width, uint32_t height);
		RenderTargetPtr createRenderTarget(ImagePtr& image);
		void setRenderTarget(const std::string& targetname, bool discard, int32_t ndraws = 0);
		void render();
	};
}