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

// Standard C++ library includes
#include <SDL.h>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"
#include "video/animation.h"
#include "video/fonts/abstractfont.h"
#include "video/imagemanager.h"
#include "video/image.h"
#include "video/opengl/glimage.h"
#include "util/math/fife_math.h"
#include "util/log/logger.h"
#include "util/time/timemanager.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/timeprovider.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view/camera.h"
#include "lightrenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);

	LightRendererImageInfo::LightRendererImageInfo(RendererNode anchor, int32_t image, int32_t src, int32_t dst):
		LightRendererElementInfo(),
		m_anchor(anchor),
		m_image(image),
		m_src(src),
		m_dst(dst),
		m_stencil(false),
		m_stencil_ref(0),
		m_alpha_ref(0.0) {
	}
	void LightRendererImageInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
//prock - 504
			ImagePtr img = ImageManager::instance()->get(m_image);
			Rect r;
			Rect viewport = cam->getViewPort();
			uint32_t widtht = round(img->getWidth() * cam->getZoom());
			uint32_t height = round(img->getHeight() * cam->getZoom());
			r.x = p.x-widtht/2;
			r.y = p.y-height/2;
			r.w = widtht;
			r.h = height;

			if(r.intersects(viewport)) {
				img->render(r);
				uint8_t lm = renderbackend->getLightingModel();
				if (m_stencil) {
					renderbackend->changeRenderInfos(1, m_src, m_dst, false, m_stencil, m_stencil_ref, INCR, GEQUAL);
				} else if (lm == 1) {
					renderbackend->changeRenderInfos(1, m_src, m_dst, false, m_stencil, 255, KEEP, NOTEQUAL);
				} else if (lm == 2) {
					renderbackend->changeRenderInfos(1, m_src, m_dst, false, m_stencil, 0, REPLACE, GEQUAL);
				}
			}
		}
	}
	void LightRendererImageInfo::setStencil(uint8_t stencil_ref, float alpha_ref) {
		m_stencil = true;
		m_stencil_ref = stencil_ref;
		m_alpha_ref = alpha_ref;
	}
	int32_t LightRendererImageInfo::getStencil() {
		if(!m_stencil) {
			return -1;
		}
		return m_stencil_ref;
	}
	float LightRendererImageInfo::getAlpha() {
		return m_alpha_ref;
	}
	void LightRendererImageInfo::removeStencil() {
		m_stencil = false;
		m_stencil_ref = 0;
		m_alpha_ref = 0.0;
	}

	LightRendererAnimationInfo::LightRendererAnimationInfo(RendererNode anchor, int32_t animation, int32_t src, int32_t dst):
		LightRendererElementInfo(),
		m_anchor(anchor),
		m_animation(animation),
		m_src(src),
		m_dst(dst),
		m_start_time(TimeManager::instance()->getTime()),
		m_time_scale(1.0),
		m_stencil(false),
		m_stencil_ref(0),
		m_alpha_ref(0.0) {
	}
	void LightRendererAnimationInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
//prock - 504
//			Animation& animation = animpool->getAnimation(m_animation);
//@fixme must be able to set the animation to use.  This is currently broken.
			Animation animation;
			int32_t animtime = scaleTime(m_time_scale, TimeManager::instance()->getTime() - m_start_time) % animation.getDuration();
			ImagePtr img = ImageManager::instance()->get(animation.getFrameByTimestamp(animtime));
			Rect r;
			Rect viewport = cam->getViewPort();
			uint32_t widtht = round(img->getWidth() * cam->getZoom());
			uint32_t height = round(img->getHeight() * cam->getZoom());
			r.x = p.x-widtht/2;
			r.y = p.y-height/2;
			r.w = widtht;
			r.h = height;

			if(r.intersects(viewport)) {
				img->render(r);
				uint8_t lm = renderbackend->getLightingModel();
				if (m_stencil) {
					renderbackend->changeRenderInfos(1, m_src, m_dst, false, m_stencil, m_stencil_ref, INCR, GEQUAL);
				} else if (lm == 1) {
					renderbackend->changeRenderInfos(1, m_src, m_dst, false, m_stencil, 255, KEEP, NOTEQUAL);
				} else if (lm == 2) {
					renderbackend->changeRenderInfos(1, m_src, m_dst, false, m_stencil, 0, REPLACE, GEQUAL);
				}
			}
		}
	}
	void LightRendererAnimationInfo::setStencil(uint8_t stencil_ref, float alpha_ref) {
		m_stencil = true;
		m_stencil_ref = stencil_ref;
		m_alpha_ref = alpha_ref;
	}
	int32_t LightRendererAnimationInfo::getStencil() {
		if(!m_stencil) {
			return -1;
		}
		return m_stencil_ref;
	}
	float LightRendererAnimationInfo::getAlpha() {
		return m_alpha_ref;
	}
	void LightRendererAnimationInfo::removeStencil() {
		m_stencil = false;
		m_stencil_ref = 0;
		m_alpha_ref = 0.0;
	}

	LightRendererResizeInfo::LightRendererResizeInfo(RendererNode anchor, int32_t image, int32_t width, int32_t height, int32_t src, int32_t dst):
		LightRendererElementInfo(),
		m_anchor(anchor),
		m_image(image),
		m_width(width),
		m_height(height),
		m_src(src),
		m_dst(dst),
		m_stencil(false),
		m_stencil_ref(0),
		m_alpha_ref(0.0) {
	}
	void LightRendererResizeInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
//prock - 504
//			Image* img = &imagepool->getImage(m_image);
			Image* img = NULL;
			Rect r;
			Rect viewport = cam->getViewPort();
			uint32_t widtht = round(m_width * cam->getZoom());
			uint32_t height = round(m_height * cam->getZoom());
			r.x = p.x-widtht/2;
			r.y = p.y-height/2;
			r.w = widtht;
			r.h = height;

			if(r.intersects(viewport)) {
				img->render(r);
				uint8_t lm = renderbackend->getLightingModel();
				if (m_stencil) {
					renderbackend->changeRenderInfos(1, m_src, m_dst, false, m_stencil, m_stencil_ref, INCR, GEQUAL);
				} else if (lm == 1) {
					renderbackend->changeRenderInfos(1, m_src, m_dst, false, m_stencil, 255, KEEP, NOTEQUAL);
				} else if (lm == 2) {
					renderbackend->changeRenderInfos(1, m_src, m_dst, false, m_stencil, 0, REPLACE, GEQUAL);
				}
			}
		}
	}
	void LightRendererResizeInfo::setStencil(uint8_t stencil_ref, float alpha_ref) {
		m_stencil = true;
		m_stencil_ref = stencil_ref;
		m_alpha_ref = alpha_ref;
	}
	int32_t LightRendererResizeInfo::getStencil() {
		if(!m_stencil) {
			return -1;
		}
		return m_stencil_ref;
	}
	float LightRendererResizeInfo::getAlpha() {
		return m_alpha_ref;
	}
	void LightRendererResizeInfo::removeStencil() {
		m_stencil = false;
		m_stencil_ref = 0;
		m_alpha_ref = 0.0;
	}

	LightRendererSimpleLightInfo::LightRendererSimpleLightInfo(RendererNode anchor, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t r, uint8_t g, uint8_t b, int32_t src, int32_t dst):
		LightRendererElementInfo(),
		m_anchor(anchor),
		m_intensity(intensity),
		m_radius(radius),
		m_subdivisions(subdivisions),
		m_xstretch(xstretch),
		m_ystretch(ystretch),
		m_red(r),
		m_green(g),
		m_blue(b),
		m_src(src),
		m_dst(dst),
		m_stencil(false),
		m_stencil_ref(0),
		m_alpha_ref(0.0) {
	}
	void LightRendererSimpleLightInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) {
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
			double zoom = cam->getZoom();
			renderbackend->drawLightPrimitive(p, m_intensity, m_radius, m_subdivisions, m_xstretch * zoom, m_ystretch * zoom, m_red, m_green, m_blue);

			uint8_t lm = renderbackend->getLightingModel();
			if (m_stencil) {
				renderbackend->changeRenderInfos(m_subdivisions, m_src, m_dst, false, m_stencil, m_stencil_ref, INCR, GEQUAL);
			} else if (lm == 1) {
				renderbackend->changeRenderInfos(m_subdivisions, m_src, m_dst, false, m_stencil, 255, KEEP, NOTEQUAL);
			} else if (lm == 2) {
				renderbackend->changeRenderInfos(m_subdivisions, m_src, m_dst, false, m_stencil, 0, REPLACE, GEQUAL);
			}
		}
	}
	void LightRendererSimpleLightInfo::setStencil(uint8_t stencil_ref, float alpha_ref) {
		m_stencil = true;
		m_stencil_ref = stencil_ref;
		m_alpha_ref = alpha_ref;
	}
	int32_t LightRendererSimpleLightInfo::getStencil() {
		if(!m_stencil) {
			return -1;
		}
		return m_stencil_ref;
	}
	float LightRendererSimpleLightInfo::getAlpha() {
		return m_alpha_ref;
	}
	void LightRendererSimpleLightInfo::removeStencil() {
		m_stencil = false;
		m_stencil_ref = 0;
		m_alpha_ref = 0.0;
	}
	std::vector<uint8_t> LightRendererSimpleLightInfo::getColor() {
		std::vector<uint8_t> colors;
		colors.push_back(m_red);
		colors.push_back(m_green);
		colors.push_back(m_blue);
		colors.push_back(m_intensity);
		return colors;
	}

	LightRenderer* LightRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<LightRenderer*>(cnt->getRenderer("LightRenderer"));
	}

	LightRenderer::LightRenderer(RenderBackend* renderbackend, int32_t position):
		RendererBase(renderbackend, position),
		m_groups() {
		setEnabled(false);
	}

	LightRenderer::LightRenderer(const LightRenderer& old):
		RendererBase(old),
		m_groups() {
		setEnabled(false);
	}

	RendererBase* LightRenderer::clone() {
		return new LightRenderer(*this);
	}

	LightRenderer::~LightRenderer() {
	}
	// Add a static lightmap
	void LightRenderer::addImage(const std::string &group, RendererNode n, int32_t image, int32_t src, int32_t dst) {
		LightRendererElementInfo* info = new LightRendererImageInfo(n, image, src, dst);
		m_groups[group].push_back(info);
	}
	// Add a animation lightmap
	void LightRenderer::addAnimation(const std::string &group, RendererNode n, int32_t animation, int32_t src, int32_t dst) {
		LightRendererElementInfo* info = new LightRendererAnimationInfo(n, animation, src, dst);
		m_groups[group].push_back(info);
	}
	// Add a simple light
	void LightRenderer::addSimpleLight(const std::string &group, RendererNode n, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t r, uint8_t g, uint8_t b, int32_t src, int32_t dst) {
		LightRendererElementInfo* info = new LightRendererSimpleLightInfo(n, intensity, radius, subdivisions, xstretch, ystretch, r, g, b, src, dst);
		m_groups[group].push_back(info);
	}
	// Resize an Image
	void LightRenderer::resizeImage(const std::string &group, RendererNode n, int32_t image, int32_t width, int32_t height, int32_t src, int32_t dst) {
		LightRendererElementInfo* info = new LightRendererResizeInfo(n, image, width, height, src, dst);
		m_groups[group].push_back(info);
	}
	// Enable stencil test for the group
	void LightRenderer::addStencilTest(const std::string &group, uint8_t stencil_ref, float alpha_ref) {
		std::vector<LightRendererElementInfo*>::const_iterator info_it = m_groups[group].begin();
		for (;info_it != m_groups[group].end(); ++info_it) {
			(*info_it)->setStencil(stencil_ref, alpha_ref);
		}
	}
	// Disable stencil test for the group
	void LightRenderer::removeStencilTest(const std::string &group) {
		std::vector<LightRendererElementInfo*>::const_iterator info_it = m_groups[group].begin();
		for (;info_it != m_groups[group].end(); ++info_it) {
			(*info_it)->removeStencil();
		}
	}
	// Return a list of all groups
	std::list<std::string> LightRenderer::getGroups() {
		std::list<std::string> groups;
		std::map<std::string, std::vector<LightRendererElementInfo*> >::iterator group_it = m_groups.begin();
		for(; group_it != m_groups.end(); ++group_it) {
			groups.push_back(group_it->first);
		}
		groups.sort();
		groups.unique();
		return groups;
	}
	// Return a vector of all LightElementInfos
	std::vector<LightRendererElementInfo*> LightRenderer::getLightInfo(const std::string &group) {
		std::vector<LightRendererElementInfo*> info;
		std::vector<LightRendererElementInfo*>::const_iterator info_it = m_groups[group].begin();
		for (;info_it != m_groups[group].end(); ++info_it) {
			info.push_back(*info_it);
		}
		return info;
	}
	// Remove the group
	void LightRenderer::removeAll(const std::string &group) {
		std::vector<LightRendererElementInfo*>::const_iterator info_it = m_groups[group].begin();
		for (;info_it != m_groups[group].end(); ++info_it) {
			delete *info_it;
		}
		m_groups[group].clear();
		m_groups.erase(group);
	}
	// Remove all groups
	void LightRenderer::removeAll() {
		m_groups.clear();
	}
	// Clear all groups
	void LightRenderer::reset() {
		removeAll();
	}
	// Render
	void LightRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		uint8_t lm = m_renderbackend->getLightingModel();

		if (!layer->areInstancesVisible()) {
			return;
		}

		std::map<std::string, std::vector<LightRendererElementInfo*> >::iterator group_it = m_groups.begin();
		for (; group_it != m_groups.end(); ++group_it) {
			std::vector<LightRendererElementInfo*>::const_iterator info_it = group_it->second.begin();
			for (;info_it != group_it->second.end(); ++info_it) {
				if (lm != 0) {
					if ((*info_it)->getStencil() != -1 && (*info_it)->getStencil() < 255) {
						if(info_it != group_it->second.begin()) {
							(*info_it)->setStencil((*info_it)->getStencil()+1, (*info_it)->getAlpha());
						}
					}
				}
				(*info_it)->render(cam, layer, instances, m_renderbackend);
			}
		}
	}

}
