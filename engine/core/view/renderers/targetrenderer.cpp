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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"
#include "video/imagemanager.h"
#include "util/log/logger.h"
#include "view/camera.h"

#include "targetrenderer.h"


namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIEWVIEW);

	RenderTarget::RenderTarget(RenderBackend* rb, const std::string& name, uint32_t width, uint32_t height):
		m_renderbackend(rb) {
		m_target = ImageManager::instance()->loadBlank(name, width, height);
	}

	RenderTarget::RenderTarget(RenderBackend* rb, ImagePtr& image):
		m_renderbackend(rb), m_target(image) {
	}

	RenderTarget::~RenderTarget() {
	}

	void RenderTarget::addLine(const std::string &group, Point n1, Point n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		OffRendererElementInfo* info = new OffRendererLineInfo(n1, n2, r, g, b, a);
		m_groups[group].push_back(info);
	}

	void RenderTarget::addPoint(const std::string &group, Point n, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		OffRendererElementInfo* info = new OffRendererPointInfo(n, r, g, b, a);
		m_groups[group].push_back(info);
	}

	void RenderTarget::addTriangle(const std::string &group, Point n1, Point n2, Point n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		OffRendererElementInfo* info = new OffRendererTriangleInfo(n1, n2, n3, r, g, b, a);
		m_groups[group].push_back(info);
	}

	void RenderTarget::addQuad(const std::string &group, Point n1, Point n2, Point n3, Point n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		OffRendererElementInfo* info = new OffRendererQuadInfo(n1, n2, n3, n4, r, g, b, a);
		m_groups[group].push_back(info);
	}

	void RenderTarget::addVertex(const std::string &group, Point n, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		OffRendererElementInfo* info = new OffRendererVertexInfo(n, size, r, g, b, a);
		m_groups[group].push_back(info);
	}

	void RenderTarget::addText(const std::string &group, Point n, IFont* font, const std::string &text) {
		OffRendererElementInfo* info = new OffRendererTextInfo(n, font, text);
		m_groups[group].push_back(info);
	}

	void RenderTarget::addImage(const std::string &group, Point n, ImagePtr image) {
		OffRendererElementInfo* info = new OffRendererImageInfo(n, image);
		m_groups[group].push_back(info);
	}

	void RenderTarget::addAnimation(const std::string &group, Point n, AnimationPtr animation) {
		OffRendererElementInfo* info = new OffRendererAnimationInfo(n, animation);
		m_groups[group].push_back(info);
	}

	void RenderTarget::resizeImage(const std::string &group, Point n, ImagePtr image, int32_t width, int32_t height) {
		OffRendererElementInfo* info = new OffRendererResizeInfo(n, image, width, height);
		m_groups[group].push_back(info);
	}

	void RenderTarget::removeAll(const std::string &group) {
		std::vector<OffRendererElementInfo*>::const_iterator info_it = m_groups[group].begin();
		for (;info_it != m_groups[group].end(); ++info_it) {
			delete *info_it;
		}
		m_groups[group].clear();
		m_groups.erase(group);
	}

	void RenderTarget::removeAll() {
		m_groups.clear();
	}

	void RenderTarget::render() {
		std::map<std::string, std::vector<OffRendererElementInfo*> >::iterator group_it = m_groups.begin();
		for(; group_it != m_groups.end(); ++group_it) {
			std::vector<OffRendererElementInfo*>::const_iterator info_it = group_it->second.begin();
			for (; info_it != group_it->second.end(); ++info_it) {
				(*info_it)->render(m_renderbackend);
			}
		}
	}

	TargetRenderer::TargetRenderer(RenderBackend* renderbackend) :
		m_renderbackend(renderbackend) {
	}

	TargetRenderer::~TargetRenderer() {
	}

	RenderTargetPtr TargetRenderer::createRenderTarget(const std::string& name, uint32_t width, uint32_t height) {
		RenderJob rj;
		rj.ndraws = -1;
		rj.lasttime_draw = 1;
		rj.target = RenderTargetPtr(new RenderTarget(m_renderbackend, name, width, height));
		rj.discard = false;

		std::pair<RenderJobMap::iterator, bool> ret =
			m_targets.insert(std::make_pair(name, rj));

		return ret.first->second.target;
	}

	RenderTargetPtr TargetRenderer::createRenderTarget(ImagePtr& image) {
		RenderJob rj;
		rj.ndraws = -1;
		rj.lasttime_draw = 1;
		rj.target = RenderTargetPtr(new RenderTarget(m_renderbackend, image));
		rj.discard = false;

		std::pair<RenderJobMap::iterator, bool> ret =
			m_targets.insert(std::make_pair(image->getName(), rj));

		return ret.first->second.target;
	}

	void TargetRenderer::setRenderTarget(const std::string& targetname, bool discard, int32_t ndraws) {
		RenderJobMap::iterator it = m_targets.find(targetname);
		if (it != m_targets.end()) {
			it->second.ndraws = ndraws;
			it->second.discard = discard;
		}
	}

	void TargetRenderer::render() {
		if (!m_targets.empty()) {
			for (RenderJobMap::iterator it = m_targets.begin(); it != m_targets.end(); ++it) {
				if (it->second.ndraws != -1) {
					if (it->second.ndraws <= it->second.lasttime_draw) {
						RenderTargetPtr rt = it->second.target;
						m_renderbackend->attachRenderTarget(rt->m_target, it->second.discard);
						rt->render();
						m_renderbackend->detachRenderTarget();

						if(it->second.ndraws == 0) {
							it->second.ndraws = -1;
						} else {
							it->second.lasttime_draw = 1;
						}
					} else {
						++it->second.lasttime_draw;
					}
				}
			}
		}
	}
}
