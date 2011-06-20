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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"
#include "video/image.h"
#include "video/sdl/sdlimage.h"
#include "video/animation.h"
#include "util/math/fife_math.h"
#include "util/log/logger.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/action.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "video/opengl/fife_opengl.h"

#include "view/camera.h"
#include "view/visual.h"
#include "instancerenderer.h"


namespace {
	uint32_t scale(uint32_t val, double factor) {
		return static_cast<uint32_t>(ceil(static_cast<double>(val) * factor));
	}
}

namespace FIFE {
	static Logger _log(LM_VIEWVIEW);

	InstanceRenderer::OutlineInfo::OutlineInfo():
		r(0),
		g(0),
		b(0),
		width(1),
		dirty(false),
		outline(NULL),
		curimg(NULL) {
	}
	InstanceRenderer::ColoringInfo::ColoringInfo():
		r(0),
		g(0),
		b(0) {
	}

	InstanceRenderer::AreaInfo::AreaInfo():
		instance(NULL),
		groups(),
		w(1),
		h(1),
		trans(0),
		front(true),
		z(0) {
	}

	InstanceRenderer::OutlineInfo::~OutlineInfo() {
		delete outline;
	}

	InstanceRenderer::ColoringInfo::~ColoringInfo() {
	}

	InstanceRenderer::AreaInfo::~AreaInfo() {
	}

	InstanceRenderer* InstanceRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<InstanceRenderer*>(cnt->getRenderer("InstanceRenderer"));
	}

	InstanceRenderer::InstanceRenderer(RenderBackend* renderbackend, int32_t position):
		RendererBase(renderbackend, position),
		m_area_layer(false) {
		setEnabled(true);
	}

 	InstanceRenderer::InstanceRenderer(const InstanceRenderer& old):
		RendererBase(old),
		m_area_layer(old.m_area_layer) {
		setEnabled(true);
	}

	RendererBase* InstanceRenderer::clone() {
		return new InstanceRenderer(*this);
	}

	InstanceRenderer::~InstanceRenderer() {
	}

	void InstanceRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		FL_DBG(_log, "Iterating layer...");
		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, cannot draw instances");
			return;
		}

		const bool any_effects = !(m_instance_outlines.empty() && m_instance_colorings.empty());
		const bool unlit = !m_unlit_groups.empty();
		uint32_t lm = m_renderbackend->getLightingModel();

		m_area_layer = false;
		if(!m_instance_areas.empty()) {
			InstanceToAreas_t::iterator area_it = m_instance_areas.begin();
			for(;area_it != m_instance_areas.end(); area_it++) {
				AreaInfo& info = area_it->second;
				if(info.instance->getLocation().getLayer() == layer) {
					if(info.front) {
						DoublePoint3D instance_posv = cam->toVirtualScreenCoordinates(info.instance->getLocation().getMapCoordinates());
						info.z = instance_posv.z;
					}
					m_area_layer = true;
				}
			}
		}
		
		typedef std::pair<RenderItem*, ColoringInfo*> DeferredColored;
		typedef std::pair<RenderItem*, OutlineInfo*> DeferredOutline;
		static std::vector<DeferredColored> coloredList;
		static std::vector<DeferredOutline> outlineList;

		RenderList::iterator instance_it = instances.begin();
		for (;instance_it != instances.end(); ++instance_it) {
			FL_DBG(_log, "Iterating instances...");
			Instance* instance = (*instance_it)->instance;
			RenderItem& vc = **instance_it;

			if(m_area_layer) {
				InstanceToAreas_t::iterator areas_it = m_instance_areas.begin();
				for(;areas_it != m_instance_areas.end(); areas_it++) {
					AreaInfo& infoa = areas_it->second;
					if(infoa.front) {
						if(infoa.z >= vc.screenpoint.z) {
							continue;
						}
					}

					std::string str_name = instance->getObject()->getNamespace();
					std::list<std::string>::iterator group_it = infoa.groups.begin();
					for(;group_it != infoa.groups.end(); ++group_it) {
						if(str_name.find((*group_it)) != std::string::npos) {
							ScreenPoint p;
							Rect rec;
							p = cam->toScreenCoordinates(infoa.instance->getLocation().getMapCoordinates());
							rec.x = p.x - infoa.w / 2;
							rec.y = p.y - infoa.h / 2;
							rec.w = infoa.w;
							rec.h = infoa.h;
							if(infoa.instance != instance && vc.dimensions.intersects(rec)) {
								vc.transparency = 255 - infoa.trans;
							}
						}
					}
				}
			}

			FL_DBG(_log, LMsg("Instance layer coordinates = ") << instance->getLocationRef().getLayerCoordinates());

			if (any_effects) {
				InstanceToOutlines_t::iterator outline_it = m_instance_outlines.find(instance);
				if (outline_it != m_instance_outlines.end()) {
					// defer outlines until whole layer is rendered
					outlineList.push_back(std::make_pair(*instance_it, &outline_it->second));
					continue;
				}

				InstanceToColoring_t::iterator coloring_it = m_instance_colorings.find(instance);
				if (coloring_it != m_instance_colorings.end()) {
					// defer colored overlays until whole layer is rendered
					coloredList.push_back(std::make_pair(*instance_it, &coloring_it->second));
					continue;
				}
			}
			if(lm != 0) {
				if(unlit) {
					bool found = false;
					std::string lit_name = instance->getObject()->getNamespace();
					std::list<std::string>::iterator unlit_it = m_unlit_groups.begin();
					for(;unlit_it != m_unlit_groups.end(); ++unlit_it) {
						if(lit_name.find(*unlit_it) != std::string::npos) {
							found = true;
							break;
						}
					}
					vc.image->render(vc.dimensions, vc.transparency);
					if (found) {
						m_renderbackend->changeRenderInfos(1, 4, 5, true, true, 255, REPLACE, ALWAYS);
					} else {
						m_renderbackend->changeRenderInfos(1, 4, 5, true, true, 0, ZERO, ALWAYS);
					}
					continue;
				}
			}
			vc.image->render(vc.dimensions, vc.transparency);

		}

		// first render deferred overlays
		for(std::vector<DeferredColored>::iterator it = coloredList.begin();
			it != coloredList.end(); ++it) {
				uint8_t rgb[3] = {
					it->second->r,
					it->second->g,
					it->second->b };
				it->first->image->render(it->first->dimensions, it->first->transparency, rgb);
		}
		m_renderbackend->changeRenderInfos(coloredList.size(), 4, 5, true, false, 0, KEEP, ALWAYS);
		coloredList.clear();

		// then render deferred outlines	
		for(std::vector<DeferredOutline>::iterator it = outlineList.begin();
			it != outlineList.end(); ++it) {
				bindOutline(*it->second, *it->first, cam)->render(it->first->dimensions, it->first->transparency);
		}

		if(lm != 0) {
			m_renderbackend->changeRenderInfos(outlineList.size(), 4, 5, false, true, 255, REPLACE, ALWAYS);
			for(std::vector<DeferredOutline>::iterator it = outlineList.begin();
				it != outlineList.end(); ++it) {
					it->first->image->render(it->first->dimensions, it->first->transparency);
			}
			m_renderbackend->changeRenderInfos(outlineList.size(), 4, 5, true, true, 0, REPLACE, ALWAYS);
		}
		outlineList.clear();

		m_renderbackend->renderVertexArrays();
	}

	Image* InstanceRenderer::bindOutline(OutlineInfo& info, RenderItem& vc, Camera* cam) {
		if (!info.dirty && info.curimg == vc.image.get()) {
			// optimization for outline that has not changed
			return info.outline;
		} else {
			info.curimg = vc.image.get();
		}

		if (info.outline) {
			delete info.outline; // delete old mask
			info.outline = NULL;
		}

		SDL_Surface* surface = vc.image->getSurface();
		SDL_Surface* outline_surface;
		
		if(vc.image->isSharedImage()) {
			outline_surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA,
				vc.image->getWidth(), vc.image->getHeight(), 32,
				RMASK, GMASK, BMASK, AMASK);
			SDL_SetAlpha(surface, 0, 0);
 
			// can't directly cast Rect to SDL_Rect as int32_t isn't compatible with Sint16 or Uint16
			const Rect& rect = vc.image->getSubImageRect();
			SDL_Rect srcrect = { rect.x, rect.y, rect.w, rect.h };
			SDL_BlitSurface(surface, &srcrect, outline_surface, NULL);
			SDL_SetAlpha(surface, SDL_SRCALPHA, 0);
		} else {
			outline_surface = SDL_ConvertSurface(surface, surface->format, surface->flags);
		}

		// TODO: optimize...
		uint8_t r, g, b, a = 0;

		// vertical sweep
		for (int32_t x = 0; x < outline_surface->w; x ++) {
			uint8_t prev_a = 0;
			for (int32_t y = 0; y < outline_surface->h; y ++) {
				vc.image->getPixelRGBA(x, y, &r, &g, &b, &a);
				if ((a == 0 || prev_a == 0) && (a != prev_a)) {
					if (a < prev_a) {
						for (int32_t yy = y; yy < y + info.width; yy++) {
							Image::putPixel(outline_surface, x, yy, info.r, info.g, info.b);
						}
					} else {
						for (int32_t yy = y - info.width; yy < y; yy++) {
							Image::putPixel(outline_surface, x, yy, info.r, info.g, info.b);
						}
					}
				}
				prev_a = a;
			}
		}
		// horizontal sweep
		for (int32_t y = 0; y < outline_surface->h; y ++) {
			uint8_t prev_a = 0;
			for (int32_t x = 0; x < outline_surface->w; x ++) {
				vc.image->getPixelRGBA(x, y, &r, &g, &b, &a);
				if ((a == 0 || prev_a == 0) && (a != prev_a)) {
					if (a < prev_a) {
						for (int32_t xx = x; xx < x + info.width; xx++) {
							Image::putPixel(outline_surface, xx, y, info.r, info.g, info.b);
						}
					} else {
						for (int32_t xx = x - info.width; xx < x; xx++) {
							Image::putPixel(outline_surface, xx, y, info.r, info.g, info.b);
						}
					}
				}
				prev_a = a;
			}
		}

		// In case of OpenGL backend, SDLImage needs to be converted
		info.outline = m_renderbackend->createImage(outline_surface);

		if (info.outline) {
			// mark outline as not dirty since we created it here
			info.dirty = false;
		}

		return info.outline;
	}

	void InstanceRenderer::addOutlined(Instance* instance, int32_t r, int32_t g, int32_t b, int32_t width) {
		OutlineInfo newinfo;
		newinfo.r = r;
		newinfo.g = g;
		newinfo.b = b;
		newinfo.width = width;
		newinfo.dirty = true;

		// attempts to insert the element into the outline map
		// will return false in the second value of the pair if the instance already exists
		// in the map and the first value of the pair will then be an iterator to the
		// existing data for the instance
		std::pair<InstanceToOutlines_t::iterator, bool> insertiter = m_instance_outlines.insert(std::make_pair(instance, newinfo));

		if (insertiter.second == false) {
			// the insertion did not happen because the instance
			// already exists in the map so lets just update its outline info
			OutlineInfo& info = insertiter.first->second;

			if (info.r != r || info.g != g || info.b != b || info.width != width) {
				// only update the outline info if its changed since the last call
				// flag the outline info as dirty so it will get processed during rendering
				info.r = r;
				info.b = b;
				info.g = g;
				info.width = width;
				info.dirty = true;
			}
		}
	}

	void InstanceRenderer::addColored(Instance* instance, int32_t r, int32_t g, int32_t b) {
		ColoringInfo newinfo;
		newinfo.r = r;
		newinfo.g = g;
		newinfo.b = b;

		// attempts to insert the element into the coloring map
		// will return false in the second value of the pair if the instance already exists
		// in the map and the first value of the pair will then be an iterator to the
		// existing data for the instance
		std::pair<InstanceToColoring_t::iterator, bool> insertiter = m_instance_colorings.insert(std::make_pair(instance, newinfo));

		if (insertiter.second == false) {
			// the insertion did not happen because the instance
			// already exists in the map so lets just update its coloring info
			ColoringInfo& info = insertiter.first->second;

			if (info.r != r || info.g != g || info.b != b) {
				// only update the coloring info if its changed since the last call
				info.r = r;
				info.b = b;
				info.g = g;
			}
		}
	}

	void InstanceRenderer::addTransparentArea(Instance* instance, const std::list<std::string> &groups, uint32_t w, uint32_t h, uint8_t trans, bool front) {
		AreaInfo newinfo;
		newinfo.instance = instance;
		newinfo.groups = groups;

		newinfo.w = w;
		newinfo.h = h;
		newinfo.trans = trans;
		newinfo.front = front;


		// attempts to insert the element into the area map
		// will return false in the second value of the pair if the instance already exists
		// in the map and the first value of the pair will then be an iterator to the
		// existing data for the instance
		std::pair<InstanceToAreas_t::iterator, bool> insertiter = m_instance_areas.insert(std::make_pair(instance, newinfo));

		if (insertiter.second == false) {
			// the insertion did not happen because the instance
			// already exists in the map so lets just update its area info
			AreaInfo& info = insertiter.first->second;
		}
	}

	void InstanceRenderer::removeOutlined(Instance* instance) {
		m_instance_outlines.erase(instance);
	}

	void InstanceRenderer::removeColored(Instance* instance) {
		m_instance_colorings.erase(instance);
	}

	void InstanceRenderer::removeTransparentArea(Instance* instance) {
		m_instance_areas.erase(instance);
	}

	void InstanceRenderer::removeAllOutlines() {
		m_instance_outlines.clear();
	}

	void InstanceRenderer::removeAllColored() {
		m_instance_colorings.clear();
	}

	void InstanceRenderer::removeAllTransparentAreas() {
		m_instance_areas.clear();
	}

	void InstanceRenderer::addIgnoreLight(const std::list<std::string> &groups) {
		std::list<std::string>::const_iterator group_it = groups.begin();
		for(;group_it != groups.end(); ++group_it) {
			m_unlit_groups.push_back(*group_it);
		}
		m_unlit_groups.sort();
		m_unlit_groups.unique();
	}

	void InstanceRenderer::removeIgnoreLight(const std::list<std::string> &groups) {
		std::list<std::string>::const_iterator group_it = groups.begin();
		for(;group_it != groups.end(); ++group_it) {
			std::list<std::string>::iterator unlit_it = m_unlit_groups.begin();
			for(;unlit_it != m_unlit_groups.end(); ++unlit_it) {
				if((*group_it).find(*unlit_it) != std::string::npos) {
					m_unlit_groups.remove(*unlit_it);
					break;
				}
			}
		}
	}

	void InstanceRenderer::removeAllIgnoreLight() {
		m_unlit_groups.clear();
	}

	void InstanceRenderer::reset() {
		removeAllOutlines();
		removeAllColored();
		removeAllTransparentAreas();
		removeAllIgnoreLight();
	}

}
