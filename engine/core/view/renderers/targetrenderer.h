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

#ifndef FIFE_TARGETRENDERER_H
#define FIFE_TARGETRENDERER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "view/renderers/offrenderer.h"

namespace FIFE {

	class RenderTarget {
		friend class TargetRenderer;
	public:
		/** Destructor.
		 */
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

		ImagePtr getTarget() { return m_target; }
	private:
		RenderTarget(RenderBackend* rb, const std::string& name, uint32_t width, uint32_t height);
		RenderTarget(RenderBackend* rb, ImagePtr& image);

		// Non copyable
		RenderTarget(const RenderTarget& rhs); /* = delete */
		RenderTarget& operator=(const RenderTarget& rhs); /* = delete */

		std::map<std::string, std::vector<OffRendererElementInfo*> > m_groups;
		RenderBackend* m_renderbackend;
		ImagePtr m_target;
	};
	typedef SharedPtr<RenderTarget> RenderTargetPtr;

	class TargetRenderer {
	public:
		/** Constructor.
		 * @param renderbackend to use
		 */
		TargetRenderer(RenderBackend* renderbackend);

		/** Destructor.
		 */
		virtual ~TargetRenderer();

		/** Creates render target
		 */
		RenderTargetPtr createRenderTarget(const std::string& name, uint32_t width, uint32_t height);
		RenderTargetPtr createRenderTarget(ImagePtr& image);

		// -1 - dont render
		// 0 - just for the next frame
		// 1 - every frame
		// 2 - every two frames, etc...
		void setRenderTarget(const std::string& targetname, bool discard, int32_t ndraws = 0);
		void render();

	private:
		struct RenderJob {
			int32_t ndraws;
			int32_t lasttime_draw;
			RenderTargetPtr target;
			bool discard;
		};
		typedef std::map<std::string, RenderJob> RenderJobMap;
		RenderJobMap m_targets;
		RenderBackend* m_renderbackend;
	};

}

#endif
