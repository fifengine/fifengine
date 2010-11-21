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

// Standard C++ library includes
#include <SDL.h>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"
#include "video/imagepool.h"
#include "video/animation.h"
#include "video/animationpool.h"
#include "video/fonts/abstractfont.h"
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

	LightRendererNode::LightRendererNode(Instance* attached_instance, const Location &relative_location, Layer* relative_layer, const Point &relative_point):
		m_instance(attached_instance),
		m_location(relative_location),
		m_layer(relative_layer),
		m_point(relative_point) {
	}
	LightRendererNode::LightRendererNode(Instance* attached_instance, const Location &relative_location, const Point &relative_point):
		m_instance(attached_instance),
		m_location(relative_location),
		m_layer(NULL),
		m_point(relative_point) {
	}
	LightRendererNode::LightRendererNode(Instance* attached_instance, Layer* relative_layer, const Point &relative_point):
		m_instance(attached_instance),
		m_location(NULL),
		m_layer(relative_layer),
		m_point(relative_point) {
	}
	LightRendererNode::LightRendererNode(Instance* attached_instance, const Point &relative_point):
		m_instance(attached_instance),
		m_location(NULL),
		m_layer(NULL),
		m_point(relative_point) {
	}
	LightRendererNode::LightRendererNode(const Location &attached_location, Layer* relative_layer, const Point &relative_point):
		m_instance(NULL),
		m_location(attached_location),
		m_layer(relative_layer),
		m_point(relative_point) {
	}
	LightRendererNode::LightRendererNode(const Location &attached_location, const Point &relative_point):
		m_instance(NULL),
		m_location(attached_location),
		m_layer(NULL),
		m_point(relative_point) {
	}
	LightRendererNode::LightRendererNode(Layer* attached_layer, const Point &relative_point):
		m_instance(NULL),
		m_location(NULL),
		m_layer(attached_layer),
		m_point(relative_point) {
	}
	LightRendererNode::LightRendererNode(const Point &attached_point):
		m_instance(NULL),
		m_location(NULL),
		m_layer(NULL),
		m_point(attached_point) {
	}
	LightRendererNode::~LightRendererNode() {
	}

	void LightRendererNode::setAttached(Instance* attached_instance, const Location &relative_location, const Point &relative_point) {
		m_instance = attached_instance;
		m_location = relative_location;
		m_point = relative_point;
	}
	void LightRendererNode::setAttached(Instance* attached_instance, const Location &relative_location) {
		m_instance = attached_instance;
		m_location = relative_location;
	}
	void LightRendererNode::setAttached(Instance* attached_instance, const Point &relative_point) {
		m_instance = attached_instance;
		m_point = relative_point;
	}
	void LightRendererNode::setAttached(Instance* attached_instance) {
		m_instance = attached_instance;
	}
	void LightRendererNode::setAttached(const Location &attached_location, const Point &relative_point) {
		m_instance = NULL;
		m_location = attached_location;
		m_point = relative_point;
	}
	void LightRendererNode::setAttached(const Location &attached_location) {
		m_instance = NULL;
		m_location = attached_location;
	}
	void LightRendererNode::setAttached(Layer* attached_layer) {
		m_layer = attached_layer;
	}
	void LightRendererNode::setAttached(const Point &attached_point) {
		m_instance = NULL;
		m_location = NULL;
		m_point = attached_point;
	}

	void LightRendererNode::setRelative(const Location &relative_location) {
		if(m_instance == NULL) {
			throw NotSupported("No instance attached.");
		}
		m_location = relative_location;
	}
	void LightRendererNode::setRelative(const Location &relative_location, Point relative_point) {
		if(m_instance == NULL) {
			throw NotSupported("No instance attached.");
		}
		m_location = relative_location;
		m_point = relative_point;
	}
	void LightRendererNode::setRelative(const Point &relative_point) {
		if(m_instance == NULL || m_location == NULL) {
			throw NotSupported("No instance or location attached.");
		}
		m_point = relative_point;
	}

	Instance* LightRendererNode::getAttachedInstance() {
		if(m_instance == NULL) {
			throw NotSupported("No instance attached.");
		}
		return m_instance;
	}
	Location LightRendererNode::getAttachedLocation() {
		if(m_instance != NULL || m_location == NULL) {
			throw NotSupported("No location attached.");
		}
		return m_location;
	}
	Layer* LightRendererNode::getAttachedLayer() {
		if(m_layer == NULL) {
			throw NotSupported("No layer attached.");
		}
		return m_layer;
	}
	Point LightRendererNode::getAttachedPoint() {
		if(m_instance != NULL || m_location != NULL) {
			throw NotSupported("No point attached.");
		}
		return m_point;
	}

	Location LightRendererNode::getOffsetLocation() {
		if(m_instance == NULL || m_location == NULL) {
			throw NotSupported("No location as offset used.");
		}
		return m_location;
	}
	Point LightRendererNode::getOffsetPoint() {
		if(m_instance == NULL && m_location == NULL) {
			throw NotSupported("No point as offset used.");
		}
		return m_point;
	}

	Instance* LightRendererNode::getInstance() {
		return m_instance;
	}
	Location LightRendererNode::getLocation() {
		return m_location;
	}
	Layer* LightRendererNode::getLayer() {
		return m_layer;
	}
	Point LightRendererNode::getPoint() {
		return m_point;
	}

	Point LightRendererNode::getCalculatedPoint(Camera* cam, Layer* layer) {
		ScreenPoint p;
		if(m_instance != NULL) {
			if(m_layer == NULL) {
				m_layer = m_instance->getLocation().getLayer();
			}
			if(m_location != NULL) {
				p = cam->toScreenCoordinates(m_instance->getLocationRef().getMapCoordinates() + m_location.getMapCoordinates());
			} else {
				p = cam->toScreenCoordinates(m_instance->getLocation().getMapCoordinates());
			}
		} else if(m_location != NULL) {
			if(m_layer == NULL) {
				m_layer = m_location.getLayer();
			}
			p = cam->toScreenCoordinates(m_location.getMapCoordinates());
		} else if(m_layer == NULL) {
			const std::list<Layer*>& layers = cam->getRenderer("LightRenderer")->getActiveLayers();
			std::list<Layer*>::const_reverse_iterator layer_it = layers.rbegin();
			setAttached(*layer_it);
		}
		return Point(m_point.x + p.x, m_point.y + p.y);
	}

	LightRendererImageInfo::LightRendererImageInfo(LightRendererNode anchor, int image, int src, int dst):
		LightRendererElementInfo(),
		m_anchor(anchor),
		m_image(image),
		m_src(src),
		m_dst(dst),
		m_stencil(false),
		m_stencil_ref(0),
		m_alpha_ref(0.0) {
	}
	void LightRendererImageInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) {
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
			Image* img = &imagepool->getImage(m_image);
			Rect r;
			Rect viewport = cam->getViewPort();
			unsigned int widtht = round(img->getWidth() * cam->getZoom());
			unsigned int height = round(img->getHeight() * cam->getZoom());
			r.x = p.x-widtht/2;
			r.y = p.y-height/2;
			r.w = widtht;
			r.h = height;
			renderbackend->changeBlending(m_src, m_dst);
			if(r.intersects(viewport))
				img->render(r);
		}
	}
	void LightRendererImageInfo::setStencil(uint8_t stencil_ref, float alpha_ref) {
		m_stencil = true;
		m_stencil_ref = stencil_ref;
		m_alpha_ref = alpha_ref;
	}
	int LightRendererImageInfo::getStencil() {
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
	
	LightRendererAnimationInfo::LightRendererAnimationInfo(LightRendererNode anchor, int animation, int src, int dst):
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
	void LightRendererAnimationInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) { 
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
			Animation& animation = animpool->getAnimation(m_animation);
			int animtime = scaleTime(m_time_scale, TimeManager::instance()->getTime() - m_start_time) % animation.getDuration();
			Image* img = animation.getFrameByTimestamp(animtime);
			Rect r;
			Rect viewport = cam->getViewPort();
			unsigned int widtht = round(img->getWidth() * cam->getZoom());
			unsigned int height = round(img->getHeight() * cam->getZoom());
			r.x = p.x-widtht/2;
			r.y = p.y-height/2;
			r.w = widtht;
			r.h = height;
			renderbackend->changeBlending(m_src, m_dst);
			if(r.intersects(viewport))
				img->render(r);
		}
	}
	void LightRendererAnimationInfo::setStencil(uint8_t stencil_ref, float alpha_ref) {
		m_stencil = true;
		m_stencil_ref = stencil_ref;
		m_alpha_ref = alpha_ref;
	}
	int LightRendererAnimationInfo::getStencil() {
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

	LightRendererResizeInfo::LightRendererResizeInfo(LightRendererNode anchor, int image, int width, int height, int src, int dst):
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
	void LightRendererResizeInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) {
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
			Image* img = &imagepool->getImage(m_image);
			Rect r;
			Rect viewport = cam->getViewPort();
			unsigned int widtht = round(m_width * cam->getZoom());
			unsigned int height = round(m_height * cam->getZoom());
			r.x = p.x-widtht/2;
			r.y = p.y-height/2;
			r.w = widtht;
			r.h = height;
			renderbackend->changeBlending(m_src, m_dst);
			if(r.intersects(viewport))
				img->render(r);
		}
	}
	void LightRendererResizeInfo::setStencil(uint8_t stencil_ref, float alpha_ref) {
		m_stencil = true;
		m_stencil_ref = stencil_ref;
		m_alpha_ref = alpha_ref;
	}
	int LightRendererResizeInfo::getStencil() {
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

	LightRendererSimpleLightInfo::LightRendererSimpleLightInfo(LightRendererNode anchor, uint8_t intensity, float radius, int subdivisions, float xstretch, float ystretch, uint8_t r, uint8_t g, uint8_t b, int src, int dst):
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
	void LightRendererSimpleLightInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) {
		Point p = m_anchor.getCalculatedPoint(cam, layer);
		if(m_anchor.getLayer() == layer) {
			double zoom = cam->getZoom();
			renderbackend->changeBlending(m_src, m_dst);
			renderbackend->drawLightPrimitive(p, m_intensity, m_radius, m_subdivisions, m_xstretch * zoom, m_ystretch * zoom, m_red, m_green, m_blue);
		}
	}
	void LightRendererSimpleLightInfo::setStencil(uint8_t stencil_ref, float alpha_ref) {
		m_stencil = true;
		m_stencil_ref = stencil_ref;
		m_alpha_ref = alpha_ref;
	}
	int LightRendererSimpleLightInfo::getStencil() {
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
	
	LightRenderer::LightRenderer(RenderBackend* renderbackend, int position, ImagePool* imagepool, AnimationPool* animpool):
		RendererBase(renderbackend, position),
		m_imagepool(imagepool),
		m_animationpool(animpool),
		m_groups() {
		setEnabled(false);
	}

	LightRenderer::LightRenderer(const LightRenderer& old):
		RendererBase(old),
		m_imagepool(old.m_imagepool),
		m_animationpool(old.m_animationpool),
		m_groups() {
		setEnabled(false);
	}

	RendererBase* LightRenderer::clone() {
		return new LightRenderer(*this);
	}

	LightRenderer::~LightRenderer() {
	}
	// Add a static lightmap
	void LightRenderer::addImage(const std::string &group, LightRendererNode n, int image, int src, int dst) {
		LightRendererElementInfo* info = new LightRendererImageInfo(n, image, src, dst);
		m_groups[group].push_back(info);
	}
	// Add a animation lightmap
	void LightRenderer::addAnimation(const std::string &group, LightRendererNode n, int animation, int src, int dst) {
		LightRendererElementInfo* info = new LightRendererAnimationInfo(n, animation, src, dst);
		m_groups[group].push_back(info);
	}
	// Add a simple light
	void LightRenderer::addSimpleLight(const std::string &group, LightRendererNode n, uint8_t intensity, float radius, int subdivisions, float xstretch, float ystretch, uint8_t r, uint8_t g, uint8_t b, int src, int dst) {
		LightRendererElementInfo* info = new LightRendererSimpleLightInfo(n, intensity, radius, subdivisions, xstretch, ystretch, r, g, b, src, dst);
		m_groups[group].push_back(info);
	}
	// Resize an Image
	void LightRenderer::resizeImage(const std::string &group, LightRendererNode n, int image, int width, int height, int src, int dst) {
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
	// Render
	void LightRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		uint8_t lm = m_renderbackend->getLightingModel();
		
		if (!layer->areInstancesVisible()) {
			return;
		}
		m_renderbackend->disableLighting();
		std::map<std::string, std::vector<LightRendererElementInfo*> >::iterator group_it = m_groups.begin();
		for (; group_it != m_groups.end(); ++group_it) {
			std::vector<LightRendererElementInfo*>::const_iterator info_it = group_it->second.begin();
			for (;info_it != group_it->second.end(); ++info_it) {
				if (lm != 0) {
					if ((*info_it)->getStencil() != -1) {
						uint8_t sref = (*info_it)->getStencil();
						float aref = (*info_it)->getAlpha();
						if(info_it != group_it->second.begin())
							sref += 1;
						m_renderbackend->setStencilTest(sref, 3, 4);
						m_renderbackend->setAlphaTest(aref);
					} else if(lm == 1) {
						m_renderbackend->setStencilTest(255, 0, 6);
						m_renderbackend->setAlphaTest(0);
					} else if(lm == 2) {
						m_renderbackend->setStencilTest(1, 2, 4);
						m_renderbackend->setAlphaTest(0);
					}
					(*info_it)->render(cam, layer, instances, m_renderbackend, m_imagepool, m_animationpool);
					m_renderbackend->disableAlphaTest();
					m_renderbackend->disableStencilTest();
				} else {
					(*info_it)->render(cam, layer, instances, m_renderbackend, m_imagepool, m_animationpool);
				}
			}
		}
		m_renderbackend->changeBlending(4, 5);
		m_renderbackend->enableLighting();
	}

}