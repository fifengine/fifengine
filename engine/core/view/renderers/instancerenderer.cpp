/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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
#include <boost/bind.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/sdl/sdlimage.h"
#include "video/animation.h"
#include "util/math/fife_math.h"
#include "util/log/logger.h"
#include "util/time/timemanager.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/action.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "model/structures/map.h"
#include "model/structures/cell.h"
#include "model/structures/cellcache.h"
#include "video/opengl/fife_opengl.h"

#include "view/camera.h"
#include "view/visual.h"
#include "cellrenderer.h"
#include "instancerenderer.h"


namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIEWVIEW);

	class InstanceRendererDeleteListener : public InstanceDeleteListener {
	public:
		InstanceRendererDeleteListener(InstanceRenderer* r)	{
			m_renderer = r;
		}
		virtual ~InstanceRendererDeleteListener() {}

		virtual void onInstanceDeleted(Instance* instance) {
			m_renderer->removeInstance(instance);
		}

	private:
		InstanceRenderer* m_renderer;
	};

	InstanceRenderer::OutlineInfo::OutlineInfo(InstanceRenderer* r):
		r(0),
		g(0),
		b(0),
		width(1),
		dirty(false),
		curimg(NULL),
		renderer(r) {
	}
	InstanceRenderer::ColoringInfo::ColoringInfo(InstanceRenderer* r):
		r(0),
		g(0),
		b(0),
		a(128),
		dirty(false),
		curimg(NULL),
		renderer(r) {
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
		renderer->addToCheck(outline);
	}

	InstanceRenderer::ColoringInfo::~ColoringInfo() {
		if (renderer->needColorBinding()) {
			renderer->addToCheck(overlay);
		}
	}

	InstanceRenderer::AreaInfo::~AreaInfo() {
	}

	InstanceRenderer* InstanceRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<InstanceRenderer*>(cnt->getRenderer("InstanceRenderer"));
	}

	InstanceRenderer::InstanceRenderer(RenderBackend* renderbackend, int32_t position):
		RendererBase(renderbackend, position),
		m_area_layer(false),
		m_interval(60*1000),
		m_timer_enabled(false) {
		setEnabled(true);
		if (m_renderbackend->getName() == "OpenGL" && m_renderbackend->isDepthBufferEnabled()) {
			m_need_sorting = false;
			m_need_bind_coloring = false;
		} else {
			m_need_sorting = true;
			if (m_renderbackend->getName() == "SDL") {
				m_need_bind_coloring = true;
			} else {
				m_need_bind_coloring = false;
			}
		}
		// init timer
		m_timer.setInterval(m_interval);
		m_timer.setCallback(boost::bind(&InstanceRenderer::check, this));
		// create delete listener
		m_delete_listener = new InstanceRendererDeleteListener(this);
	}

 	InstanceRenderer::InstanceRenderer(const InstanceRenderer& old):
		RendererBase(old),
		m_area_layer(false),
		m_interval(old.m_interval),
		m_timer_enabled(false) {
		setEnabled(true);
		if (m_renderbackend->getName() == "OpenGL" && m_renderbackend->isDepthBufferEnabled()) {
			m_need_sorting = false;
			m_need_bind_coloring = false;
		} else {
			m_need_sorting = true;
			if (m_renderbackend->getName() == "SDL") {
				m_need_bind_coloring = true;
			} else {
				m_need_bind_coloring = false;
			}
		}
		// init timer
		m_timer.setInterval(m_interval);
		m_timer.setCallback(boost::bind(&InstanceRenderer::check, this));
		// create delete listener
		m_delete_listener = new InstanceRendererDeleteListener(this);
	}

	RendererBase* InstanceRenderer::clone() {
		return new InstanceRenderer(*this);
	}

	InstanceRenderer::~InstanceRenderer() {
		// remove listener from instances
		if (!m_assigned_instances.empty()) {
			reset();
		}
		// delete listener
		delete m_delete_listener;
	}

	void InstanceRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
//		FL_DBG(_log, "Iterating layer...");
		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, cannot draw instances");
			return;
		}

		if(m_need_sorting) {
			renderAlreadySorted(cam, layer, instances);
		} else {
			renderUnsorted(cam, layer, instances);
		}
	}

	void InstanceRenderer::renderUnsorted(Camera* cam, Layer* layer, RenderList& instances) {
		// FIXME: Unlit is currently broken, maybe it would be the best to change Lightsystem
		const bool any_effects = !(m_instance_outlines.empty() && m_instance_colorings.empty());
		const bool unlit = !m_unlit_groups.empty();
		uint32_t lm = m_renderbackend->getLightingModel();
		// thanks to multimap, we will have transparent instances already sorted by their z value (key)
		std::multimap<float, RenderItem*> transparentInstances;

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
		// Fixme
		CellRenderer* cr = dynamic_cast<CellRenderer*>(cam->getRenderer("CellRenderer"));
		Layer* fow_layer = cr->getFowLayer();
		const bool check_fow = (fow_layer == layer && cr->isEnabledFogOfWar());

		RenderList::iterator instance_it = instances.begin();
		for (;instance_it != instances.end(); ++instance_it) {
//			FL_DBG(_log, "Iterating instances...");
			Instance* instance = (*instance_it)->instance;
			RenderItem& vc = **instance_it;
			float vertexZ = vc.vertexZ;

			if (check_fow) {
				Cell* cell = layer->getCellCache()->getCell(instance->getLocationRef().getLayerCoordinates());
				if (cell) {
					if (cell->getFoWType() != CELLV_REVEALED) {
						continue;
					}
				}
			}

			if (m_area_layer) {
				InstanceToAreas_t::iterator areas_it = m_instance_areas.begin();
				for(;areas_it != m_instance_areas.end(); areas_it++) {
					AreaInfo& infoa = areas_it->second;
					if (infoa.front) {
						if (infoa.z >= vc.screenpoint.z) {
							continue;
						}
					}

					std::string str_name = instance->getObject()->getNamespace();
					std::list<std::string>::iterator group_it = infoa.groups.begin();
					for (;group_it != infoa.groups.end(); ++group_it) {
						if (str_name.find((*group_it)) != std::string::npos) {
							ScreenPoint p;
							Rect rec;
							p = cam->toScreenCoordinates(infoa.instance->getLocation().getMapCoordinates());
							rec.x = p.x - infoa.w / 2;
							rec.y = p.y - infoa.h / 2;
							rec.w = infoa.w;
							rec.h = infoa.h;
							if (infoa.instance != instance && vc.dimensions.intersects(rec)) {
								vc.transparency = 255 - infoa.trans;
								// dirty hack to reset the transparency on next pump
								InstanceVisual* visual = instance->getVisual<InstanceVisual>();
								visual->setVisible(!visual->isVisible());
								visual->setVisible(!visual->isVisible());
							}
						}
					}
				}
			}

			// if instance is not opacous
			if (vc.transparency != 255) {
				transparentInstances.insert(std::pair<float, RenderItem*>(vertexZ, &vc));
				continue;
			}

			uint8_t coloringColor[4] = { 0 };
			Image* outlineImage = 0;
			bool recoloring = false;
			if (any_effects) {
				// coloring
				InstanceToColoring_t::iterator coloring_it = m_instance_colorings.find(instance);
				const bool coloring = coloring_it != m_instance_colorings.end();
				if (coloring) {
					coloringColor[0] = coloring_it->second.r;
					coloringColor[1] = coloring_it->second.g;
					coloringColor[2] = coloring_it->second.b;
					coloringColor[3] = coloring_it->second.a;
					recoloring = true;
				}
				// outline
				InstanceToOutlines_t::iterator outline_it = m_instance_outlines.find(instance);
				const bool outline = outline_it != m_instance_outlines.end();
				if (outline) {
					if (lm != 0) {
						// first render normal image without stencil and alpha test (0)
						// so it wont look aliased and then with alpha test render only outline (its 'binary' image)
						outlineImage = bindOutline(outline_it->second, vc, cam);
					} else {
						bindOutline(outline_it->second, vc, cam)->renderZ(vc.dimensions, vertexZ, vc.transparency, static_cast<uint8_t*>(0));
					}
				}
			}
//			if(lm != 0) {
//				if(unlit) {
//					bool found = false;
//					std::string lit_name = instance->getObject()->getNamespace();
//					std::list<std::string>::iterator unlit_it = m_unlit_groups.begin();
//					for(;unlit_it != m_unlit_groups.end(); ++unlit_it) {
//						if(lit_name.find(*unlit_it) != std::string::npos) {
//							found = true;
//							break;
//						}
//					}
//					vc.image->render(vc.dimensions, vc.transparency, recoloring ? coloringColor : 0);
//					if (found) {
//						m_renderbackend->changeRenderInfos(1, 4, 5, true, true, 255, REPLACE, ALWAYS, recoloring ? OVERLAY_TYPE_COLOR : OVERLAY_TYPE_NONE);
//					} else {
//						m_renderbackend->changeRenderInfos(1, 4, 5, true, true, 0, ZERO, ALWAYS, recoloring ? OVERLAY_TYPE_COLOR : OVERLAY_TYPE_NONE);
//					}
//					if (outlineImage) {
//						outlineImage->render(vc.dimensions, vc.transparency);
//						m_renderbackend->changeRenderInfos(1, 4, 5, false, true, 255, REPLACE, ALWAYS);
//					}
//					continue;
//				}
//			}
			// overlay
			if (vc.m_overlay) {
				renderOverlay(RENDER_DATA_MULTITEXTURE_Z, &vc, coloringColor, recoloring);
			// no overlay
			} else {
				vc.image->renderZ(vc.dimensions, vertexZ, vc.transparency, recoloring ? coloringColor : 0);
			}

			if (outlineImage) {
				outlineImage->renderZ(vc.dimensions, vertexZ, vc.transparency, static_cast<uint8_t*>(0));
				m_renderbackend->changeRenderInfos(RENDER_DATA_TEXTURE_Z, 1, 4, 5, false, true, 255, REPLACE, ALWAYS);
			}
		}
		// iterate through all (semi) transparent instances
		std::multimap<float, RenderItem*>::iterator it = transparentInstances.begin();
		for( ; it != transparentInstances.end(); ++it) {
			RenderItem& vc = *(it->second);
			Instance* instance = vc.instance;
			uint8_t alpha = vc.transparency;
			float vertexZ = it->first;

			uint8_t coloringColor[4] = { 0 };
			Image* outlineImage = 0;
			bool recoloring = false;
			if (any_effects) {
				// coloring
				InstanceToColoring_t::iterator coloring_it = m_instance_colorings.find(instance);
				const bool coloring = coloring_it != m_instance_colorings.end();
				if (coloring) {
					coloringColor[0] = coloring_it->second.r;
					coloringColor[1] = coloring_it->second.g;
					coloringColor[2] = coloring_it->second.b;
					coloringColor[3] = coloring_it->second.a;
					recoloring = true;
				}
				// outline
				InstanceToOutlines_t::iterator outline_it = m_instance_outlines.find(instance);
				const bool outline = outline_it != m_instance_outlines.end();
				if (outline) {
					if (lm != 0) {
						// first render normal image without stencil and alpha test (0)
						// so it wont look aliased and then with alpha test render only outline (its 'binary' image)
						outlineImage = bindOutline(outline_it->second, vc, cam);
					} else {
						bindOutline(outline_it->second, vc, cam)->renderZ(vc.dimensions, vertexZ, vc.transparency, static_cast<uint8_t*>(0));
					}
				}
			}
			// overlay
			if (vc.m_overlay) {
				renderOverlay(RENDER_DATA_MULTITEXTURE_Z, &vc, coloringColor, recoloring);
			// no overlay
			} else {
				vc.image->renderZ(vc.dimensions, vertexZ, vc.transparency, recoloring ? coloringColor : 0);
			}

			if (outlineImage) {
				outlineImage->renderZ(vc.dimensions, vertexZ, vc.transparency, static_cast<uint8_t*>(0));
				m_renderbackend->changeRenderInfos(RENDER_DATA_TEXCOLOR_Z, 1, 4, 5, false, true, 255, REPLACE, ALWAYS);
			}
		}
	}

	void InstanceRenderer::renderAlreadySorted(Camera* cam, Layer* layer, RenderList& instances) {
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
		// Fixme
		CellRenderer* cr = dynamic_cast<CellRenderer*>(cam->getRenderer("CellRenderer"));
		Layer* fow_layer = cr->getFowLayer();
		bool check_fow = (fow_layer == layer && cr->isEnabledFogOfWar());

		RenderList::iterator instance_it = instances.begin();
		for (;instance_it != instances.end(); ++instance_it) {
//			FL_DBG(_log, "Iterating instances...");
			Instance* instance = (*instance_it)->instance;
			RenderItem& vc = **instance_it;

			if (check_fow) {
				Cell* cell = layer->getCellCache()->getCell(instance->getLocationRef().getLayerCoordinates());
				if (cell) {
					if (cell->getFoWType() != CELLV_REVEALED) {
						continue;
					}
				}
			}

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
								// dirty hack to reset the transparency on next pump
								InstanceVisual* visual = instance->getVisual<InstanceVisual>();
								visual->setVisible(!visual->isVisible());
								visual->setVisible(!visual->isVisible());
							}
						}
					}
				}
			}

//			FL_DBG(_log, LMsg("Instance layer coordinates = ") << instance->getLocationRef().getLayerCoordinates());

			uint8_t coloringColor[4] = { 0 };
			Image* outlineImage = 0;
			bool recoloring = false;
			if (any_effects) {
				// coloring
				InstanceToColoring_t::iterator coloring_it = m_instance_colorings.find(instance);
				const bool coloring = coloring_it != m_instance_colorings.end();
				if (coloring && !m_need_bind_coloring) {
					coloringColor[0] = coloring_it->second.r;
					coloringColor[1] = coloring_it->second.g;
					coloringColor[2] = coloring_it->second.b;
					coloringColor[3] = coloring_it->second.a;
					recoloring = true;
				}
				// outline
				InstanceToOutlines_t::iterator outline_it = m_instance_outlines.find(instance);
				const bool outline = outline_it != m_instance_outlines.end();
				if (outline) {
					if (lm != 0) {
						// first render normal image without stencil and alpha test (0)
						// so it wont look aliased and then with alpha test render only outline (its 'binary' image)
						outlineImage = bindOutline(outline_it->second, vc, cam);
					} else {
						bindOutline(outline_it->second, vc, cam)->render(vc.dimensions, vc.transparency);
					}
				}
				// coloring for SDL
				if (coloring && m_need_bind_coloring) {
					bindColoring(coloring_it->second, vc, cam)->render(vc.dimensions, vc.transparency);
					m_renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, true, false, 0, KEEP, ALWAYS);
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
					vc.image->render(vc.dimensions, vc.transparency, recoloring ? coloringColor : 0);
					if (found) {
						m_renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, true, true, 255, REPLACE, ALWAYS, recoloring ? OVERLAY_TYPE_COLOR : OVERLAY_TYPE_NONE);
					} else {
						m_renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, true, true, 0, ZERO, ALWAYS, recoloring ? OVERLAY_TYPE_COLOR : OVERLAY_TYPE_NONE);
					}
					if (outlineImage) {
						outlineImage->render(vc.dimensions, vc.transparency);
						m_renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, true, 255, REPLACE, ALWAYS);
					}
					continue;
				}
			}
			// overlay
			if (vc.m_overlay) {
				renderOverlay(RENDER_DATA_WITHOUT_Z, &vc, coloringColor, recoloring);
			// no overlay
			} else {
				vc.image->render(vc.dimensions, vc.transparency, recoloring ? coloringColor : 0);
			}

			if (outlineImage) {
				outlineImage->render(vc.dimensions, vc.transparency);
				m_renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, true, 255, REPLACE, ALWAYS);
			}
		}
	}

	void InstanceRenderer::renderOverlay(RenderDataType type, RenderItem* item, uint8_t const* coloringColor, bool recoloring) {
		RenderItem& vc = *item;
		Instance* instance = vc.instance;
		bool withZ = type != RENDER_DATA_WITHOUT_Z;
		float vertexZ = vc.vertexZ;

		// animation overlay
		std::vector<ImagePtr>* animationOverlay = vc.getAnimationOverlay();
		// animation color overlay
		std::vector<OverlayColors*>* animationColorOverlay = vc.getAnimationColorOverlay();

		// animation overlay without color overlay
		if (animationOverlay && !animationColorOverlay) {
			if (withZ) {
				for (std::vector<ImagePtr>::iterator it = animationOverlay->begin(); it != animationOverlay->end(); ++it) {
					(*it)->renderZ(vc.dimensions, vertexZ, vc.transparency, recoloring ? coloringColor : 0);
				}
			} else {
				for (std::vector<ImagePtr>::iterator it = animationOverlay->begin(); it != animationOverlay->end(); ++it) {
					(*it)->render(vc.dimensions, vc.transparency, recoloring ? coloringColor : 0);
				}
			}
		// animation overlay with color overlay
		} else if (animationOverlay && animationColorOverlay) {
			std::vector<OverlayColors*>::iterator ovit = animationColorOverlay->begin();
			std::vector<ImagePtr>::iterator it = animationOverlay->begin();
			for (; it != animationOverlay->end(); ++it, ++ovit) {
				OverlayColors* oc = (*ovit);
				if (!oc) {
					if (withZ) {
						(*it)->renderZ(vc.dimensions, vertexZ, vc.transparency, recoloring ? coloringColor : 0);
					} else {
						(*it)->render(vc.dimensions, vc.transparency, recoloring ? coloringColor : 0);
					}
				} else {
					if (oc->getColors().size() > 1) {
						std::map<Color, Color>::const_iterator cit = oc->getColors().begin();
						uint8_t factor[4] = { 0, 0, 0, cit->second.getAlpha() };
						// multi color overlay
						ImagePtr multiColorOverlay;
						if (recoloring) {
							// create temp OverlayColors
							OverlayColors* temp = new OverlayColors(oc->getColorOverlayImage());
							float alphaFactor1 = static_cast<float>(coloringColor[3] / 255.0);
							const std::map<Color, Color>& defaultColors = oc->getColors();
							for (std::map<Color, Color>::const_iterator c_it = defaultColors.begin(); c_it != defaultColors.end(); ++c_it) {
								if (c_it->second.getAlpha() == 0) {
									continue;
								}
								float alphaFactor2 = static_cast<float>(c_it->second.getAlpha() / 255.0);
								Color c(coloringColor[0]*(1.0-alphaFactor1) + (c_it->second.getR()*alphaFactor2)*alphaFactor1,
									coloringColor[1]*(1.0-alphaFactor1) + (c_it->second.getG()*alphaFactor2)*alphaFactor1,
									coloringColor[2]*(1.0-alphaFactor1) + (c_it->second.getB()*alphaFactor2)*alphaFactor1, 255);
								temp->changeColor(c_it->first, c);
							}
							// create new factor
							factor[3] = static_cast<uint8_t>(255 - factor[3]);
							factor[3] = std::min(coloringColor[3], factor[3]);
							// get overlay image with temp colors
							multiColorOverlay = getMultiColorOverlay(vc, temp);
							delete temp;
						} else {
							multiColorOverlay = getMultiColorOverlay(vc, oc);
							factor[3] = 0;
						}
						if (withZ) {
							(*it)->renderZ(vc.dimensions, vertexZ, vc.transparency, recoloring ? coloringColor : 0);
							(*it)->renderZ(vc.dimensions, vertexZ, multiColorOverlay, vc.transparency, factor);
						} else {
							(*it)->render(vc.dimensions, vc.transparency, recoloring ? coloringColor : 0);
							(*it)->render(vc.dimensions, multiColorOverlay, vc.transparency, factor);
						}
						continue;
					}
					// single color overlay
					std::map<Color, Color>::const_iterator color_it = oc->getColors().begin();
					uint8_t rgba[4] = { color_it->second.getR(), color_it->second.getG(), color_it->second.getB(), static_cast<uint8_t>(255-color_it->second.getAlpha()) };
					bool noOverlay = rgba[3] == 255;
					if (recoloring) {
						if (!noOverlay) {
							float alphaFactor1 = static_cast<float>(coloringColor[3] / 255.0);
							float alphaFactor2 = 1.0-static_cast<float>(rgba[3] / 255.0);
							rgba[0] = coloringColor[0]*(1.0-alphaFactor1) + (rgba[0]*alphaFactor2)*alphaFactor1;
							rgba[1] = coloringColor[1]*(1.0-alphaFactor1) + (rgba[1]*alphaFactor2)*alphaFactor1;
							rgba[2] = coloringColor[2]*(1.0-alphaFactor1) + (rgba[2]*alphaFactor2)*alphaFactor1;
							rgba[3] = std::min(coloringColor[3], rgba[3]);
						}
					}
					if (withZ) {
						(*it)->renderZ(vc.dimensions, vertexZ, vc.transparency, recoloring ? coloringColor : 0);
						if (!noOverlay) {
							(*it)->renderZ(vc.dimensions, vertexZ, oc->getColorOverlayImage(), vc.transparency, rgba);
							m_renderbackend->changeRenderInfos(type, 1, 4, 5, true, false, 0, KEEP, ALWAYS, OVERLAY_TYPE_COLOR_AND_TEXTURE);
						}
					} else {
						(*it)->render(vc.dimensions, vc.transparency, recoloring ? coloringColor : 0);
						if (!noOverlay) {
							(*it)->render(vc.dimensions, oc->getColorOverlayImage(), vc.transparency, rgba);
							m_renderbackend->changeRenderInfos(type, 1, 4, 5, true, false, 0, KEEP, ALWAYS, OVERLAY_TYPE_COLOR_AND_TEXTURE);
						}
					}
				}
			}
		} else {
			OverlayColors* colorOverlay = vc.getColorOverlay();
			if (colorOverlay->getColors().size() > 1) {
				// multi color overlay
				ImagePtr multiColorOverlay;
				// interpolation factor
				std::map<Color, Color>::const_iterator it = colorOverlay->getColors().begin();
				uint8_t factor[4] = { 0, 0, 0, it->second.getAlpha() };
				if (recoloring) {
					// create temp OverlayColors
					OverlayColors* temp = new OverlayColors(colorOverlay->getColorOverlayImage());
					float alphaFactor1 = static_cast<float>(coloringColor[3] / 255.0);
					const std::map<Color, Color>& defaultColors = colorOverlay->getColors();
					for (std::map<Color, Color>::const_iterator c_it = defaultColors.begin(); c_it != defaultColors.end(); ++c_it) {
						if (c_it->second.getAlpha() == 0) {
							continue;
						}
						float alphaFactor2 = static_cast<float>(c_it->second.getAlpha() / 255.0);
						Color c(coloringColor[0]*(1.0-alphaFactor1) + (c_it->second.getR()*alphaFactor2)*alphaFactor1,
							coloringColor[1]*(1.0-alphaFactor1) + (c_it->second.getG()*alphaFactor2)*alphaFactor1,
							coloringColor[2]*(1.0-alphaFactor1) + (c_it->second.getB()*alphaFactor2)*alphaFactor1, 255);
						temp->changeColor(c_it->first, c);
					}
					// create new factor
					factor[3] = static_cast<uint8_t>(255 - factor[3]);
					factor[3] = std::min(coloringColor[3], factor[3]);
					// get overlay image with temp colors
					multiColorOverlay = getMultiColorOverlay(vc, temp);
					delete temp;
				}
				if (!multiColorOverlay) {
					multiColorOverlay = getMultiColorOverlay(vc);
					factor[3] = 0;
				}
				if (withZ) {
					vc.image->renderZ(vc.dimensions, vertexZ, vc.transparency, recoloring ? coloringColor : 0);
					vc.image->renderZ(vc.dimensions, vertexZ, multiColorOverlay, vc.transparency, factor);
				} else {
					vc.image->render(vc.dimensions, vc.transparency, recoloring ? coloringColor : 0);
					vc.image->render(vc.dimensions, multiColorOverlay, vc.transparency, factor);
				}
			} else {
				// single color overlay
				std::map<Color, Color>::const_iterator color_it = colorOverlay->getColors().begin();
				uint8_t rgba[4] = { color_it->second.getR(), color_it->second.getG(), color_it->second.getB(), static_cast<uint8_t>(255-color_it->second.getAlpha()) };
				bool noOverlay = rgba[3] == 255;
				if (recoloring) {
					if (!noOverlay) {
						float alphaFactor1 = static_cast<float>(coloringColor[3] / 255.0);
						float alphaFactor2 = 1.0-static_cast<float>(rgba[3] / 255.0);
						rgba[0] = coloringColor[0]*(1.0-alphaFactor1) + (rgba[0]*alphaFactor2)*alphaFactor1;
						rgba[1] = coloringColor[1]*(1.0-alphaFactor1) + (rgba[1]*alphaFactor2)*alphaFactor1;
						rgba[2] = coloringColor[2]*(1.0-alphaFactor1) + (rgba[2]*alphaFactor2)*alphaFactor1;
						rgba[3] = std::min(coloringColor[3], rgba[3]);
					}
				}
				if (withZ) {
					vc.image->renderZ(vc.dimensions, vertexZ, vc.transparency, recoloring ? coloringColor : 0);
					if (!noOverlay) {
						vc.image->renderZ(vc.dimensions, vertexZ, colorOverlay->getColorOverlayImage(), vc.transparency, rgba);
						m_renderbackend->changeRenderInfos(type, 1, 4, 5, true, false, 0, KEEP, ALWAYS, OVERLAY_TYPE_COLOR_AND_TEXTURE);
					}
				} else {
					vc.image->render(vc.dimensions, vc.transparency, recoloring ? coloringColor : 0);
					if (!noOverlay) {
						vc.image->render(vc.dimensions, colorOverlay->getColorOverlayImage(), vc.transparency, rgba);
						m_renderbackend->changeRenderInfos(type, 1, 4, 5, true, false, 0, KEEP, ALWAYS, OVERLAY_TYPE_COLOR_AND_TEXTURE);
					}
				}
			}
		}
	}

	inline bool aboveThreshold(int32_t threshold, int32_t alpha, int32_t prev_alpha) {
		if(threshold > 1) {
			// new behavior
			if (((alpha - threshold) >= 0 || (prev_alpha - threshold) >= 0) && (alpha != prev_alpha)) {
				return true;
			} else {
				return false;
			}
		} else {
			// old behavior
			if((alpha == 0 || prev_alpha == 0) && (alpha != prev_alpha)) {
				return true;
			} else {
				return false;
			}
		}
	}

	Image* InstanceRenderer::bindOutline(OutlineInfo& info, RenderItem& vc, Camera* cam) {
		bool valid = isValidImage(info.outline);
		if (!info.dirty && info.curimg == vc.image.get() && valid) {
			removeFromCheck(info.outline);
			// optimization for outline that has not changed
			return info.outline.get();
		} else {
			info.curimg = vc.image.get();
		}

		// if outline has changed we can maybe free the old effect image
		if (valid) {
			addToCheck(info.outline);
		}
		// special case for animation overlay
		if (vc.getAnimationOverlay()) {
			return bindMultiOutline(info, vc, cam);
		}
		// NOTE: Since r3721 outline is just the 'border' so to render everything correctly
		// we need to first render normal image, and then its outline.
		// This helps much with lighting stuff and doesn't require from us to copy image.

		bool found = false;
		// create name
		std::stringstream sts;
		sts << vc.image.get()->getName() << "," << static_cast<uint32_t>(info.r) << "," <<
			static_cast<uint32_t>(info.g) << "," << static_cast<uint32_t>(info.b) << "," << info.width;
		// search image
		if (ImageManager::instance()->exists(sts.str())) {
			info.outline = ImageManager::instance()->getPtr(sts.str());
			if (isValidImage(info.outline)) {
				removeFromCheck(info.outline);
				// mark outline as not dirty since we found it here
				info.dirty = false;
				return info.outline.get();
			}
			found = true;
		}


		// With lazy loading we can come upon a situation where we need to generate outline from
		// uninitialised shared image
		if(vc.image->isSharedImage()) {
			vc.image->forceLoadInternal();
		}

		SDL_Surface* outline_surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA,
			vc.image->getWidth(), vc.image->getHeight(), 32,
			RMASK, GMASK, BMASK, AMASK);

		// TODO: optimize...
		uint8_t r, g, b, a = 0;

		// vertical sweep
		for (int32_t x = 0; x < outline_surface->w; x ++) {
			int32_t prev_a = 0;
			for (int32_t y = 0; y < outline_surface->h; y ++) {
				vc.image->getPixelRGBA(x, y, &r, &g, &b, &a);
				if (aboveThreshold(info.threshold, static_cast<int32_t>(a), prev_a)) {
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
			int32_t prev_a = 0;
			for (int32_t x = 0; x < outline_surface->w; x ++) {
				vc.image->getPixelRGBA(x, y, &r, &g, &b, &a);
				if (aboveThreshold(info.threshold, static_cast<int32_t>(a), prev_a)) {
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
		Image* img = m_renderbackend->createImage(sts.str(), outline_surface);
		img->setState(IResource::RES_LOADED);

		if (found) {
			// image exists but is not "loaded"
			removeFromCheck(info.outline);
			ImagePtr temp(img);
			info.outline.get()->copySubimage(0, 0, temp);
			info.outline.get()->setState(IResource::RES_LOADED);
		} else {
			// create and add image
			info.outline = ImageManager::instance()->add(img);
		}
		// mark outline as not dirty since we created/recreated it here
		info.dirty = false;

		return info.outline.get();
	}

	Image* InstanceRenderer::bindMultiOutline(OutlineInfo& info, RenderItem& vc, Camera* cam) {
		// NOTE: Since r3721 outline is just the 'border' so to render everything correctly
		// we need to first render normal image, and then its outline.
		// This helps much with lighting stuff and doesn't require from us to copy image.

		bool found = false;
		// create name
		std::stringstream sts;
		uint32_t mw = 0;
		uint32_t mh = 0;
		std::vector<ImagePtr>* animationOverlays = vc.getAnimationOverlay();
		std::vector<ImagePtr>::iterator it = animationOverlays->begin();
		for (; it != animationOverlays->end(); ++it) {
			// With lazy loading we can come upon a situation where we need to generate outline from
			// uninitialised shared image
			if ((*it)->isSharedImage()) {
				(*it)->forceLoadInternal();
			}
			sts << (*it)->getName() << ",";
			mw = std::max(mw, (*it)->getWidth());
			mh = std::max(mh, (*it)->getHeight());
		}
		sts << static_cast<uint32_t>(info.r) << "," <<
			static_cast<uint32_t>(info.g) << "," << static_cast<uint32_t>(info.b) << "," << info.width;
		// search image
		if (ImageManager::instance()->exists(sts.str())) {
			info.outline = ImageManager::instance()->getPtr(sts.str());
			if (isValidImage(info.outline)) {
				removeFromCheck(info.outline);
				// mark outline as not dirty since we found it here
				info.dirty = false;
				return info.outline.get();
			}
			found = true;
		}

		SDL_Surface* outline_surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, mw, mh, 32,
			RMASK, GMASK, BMASK, AMASK);

		// TODO: optimize...
		uint8_t r, g, b, a = 0;

		it = animationOverlays->begin();
		for (; it != animationOverlays->end(); ++it) {
			// vertical sweep
			for (uint32_t x = 0; x < (*it)->getWidth(); x++) {
				int32_t prev_a = 0;
				for (uint32_t y = 0; y < (*it)->getHeight(); y++) {
					(*it)->getPixelRGBA(x, y, &r, &g, &b, &a);
					if (aboveThreshold(info.threshold, static_cast<int32_t>(a), prev_a)) {
						if (a < prev_a) {
							for (uint32_t yy = y; yy < y + info.width; yy++) {
								int32_t tx = x + (mw/2 - (*it)->getWidth()/2);
								int32_t ty = yy + (mh/2 - (*it)->getHeight()/2);
								Image::putPixel(outline_surface, tx, ty, info.r, info.g, info.b);
							}
						} else {
							for (uint32_t yy = y - info.width; yy < y; yy++) {
								int32_t tx = x + (mw/2 - (*it)->getWidth()/2);
								int32_t ty = yy + (mh/2 - (*it)->getHeight()/2);
								Image::putPixel(outline_surface, tx, ty, info.r, info.g, info.b);
							}
						}
					}
					prev_a = a;
				}
			}

			// horizontal sweep
			for (uint32_t y = 0; y < (*it)->getHeight(); y++) {
				int32_t prev_a = 0;
				for (uint32_t x = 0; x < (*it)->getWidth(); x++) {
					(*it)->getPixelRGBA(x, y, &r, &g, &b, &a);
					if (aboveThreshold(info.threshold, static_cast<int32_t>(a), prev_a)) {
						if (a < prev_a) {
							for (uint32_t xx = x; xx < x + info.width; xx++) {
								int32_t tx = xx + (mw/2 - (*it)->getWidth()/2);
								int32_t ty = y + (mh/2 - (*it)->getHeight()/2);
								Image::putPixel(outline_surface, tx, ty, info.r, info.g, info.b);
							}
						} else {
							for (uint32_t xx = x - info.width; xx < x; xx++) {
								int32_t tx = xx + (mw/2 - (*it)->getWidth()/2);
								int32_t ty = y + (mh/2 - (*it)->getHeight()/2);
								Image::putPixel(outline_surface, tx, ty, info.r, info.g, info.b);
							}
						}
					}
					prev_a = a;
				}
			}
		}

		// In case of OpenGL backend, SDLImage needs to be converted
		Image* img = m_renderbackend->createImage(sts.str(), outline_surface);
		img->setState(IResource::RES_LOADED);

		if (found) {
			// image exists but is not "loaded"
			removeFromCheck(info.outline);
			ImagePtr temp(img);
			info.outline.get()->copySubimage(0, 0, temp);
			info.outline.get()->setState(IResource::RES_LOADED);
		} else {
			// create and add image
			info.outline = ImageManager::instance()->add(img);
		}
		// mark outline as not dirty since we created/recreated it here
		info.dirty = false;

		return info.outline.get();
	}

	Image* InstanceRenderer::bindColoring(ColoringInfo& info, RenderItem& vc, Camera* cam) {
		bool valid = isValidImage(info.overlay);
		if (!info.dirty && info.curimg == vc.image.get() && valid) {
			removeFromCheck(info.overlay);
			// optimization for coloring that has not changed
			return info.overlay.get();
		} else {
			info.curimg = vc.image.get();
		}

		// if coloring has changed we can maybe free the old effect image
		if (valid) {
			addToCheck(info.overlay);
		}

		bool found = false;
		// create name
		std::stringstream sts;
		sts << vc.image.get()->getName() << "," << static_cast<uint32_t>(info.r) << "," <<
			static_cast<uint32_t>(info.g) << "," << static_cast<uint32_t>(info.b) << "," << static_cast<uint32_t>(info.a);
		// search image
		if (ImageManager::instance()->exists(sts.str())) {
			info.overlay = ImageManager::instance()->getPtr(sts.str());
			valid = isValidImage(info.overlay);
			if (valid) {
				removeFromCheck(info.overlay);
				// mark overlay as not dirty since we found it here
				info.dirty = false;
				return info.overlay.get();
			}
			found = true;
		}

		// With lazy loading we can come upon a situation where we need to generate coloring from
		// uninitialised shared image
		if(vc.image->isSharedImage()) {
			vc.image->forceLoadInternal();
		}

		// not found so we create it
		SDL_Surface* overlay_surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA,
			vc.image->getWidth(), vc.image->getHeight(), 32,
			RMASK, GMASK, BMASK, AMASK);

		uint8_t r, g, b, a = 0;
		float alphaFactor = static_cast<float>(info.a/255.0);
		for (int32_t x = 0; x < overlay_surface->w; x ++) {
			for (int32_t y = 0; y < overlay_surface->h; y ++) {
				vc.image->getPixelRGBA(x, y, &r, &g, &b, &a);
				if (a > 0) {
					Image::putPixel(overlay_surface, x, y, info.r*(1.0-alphaFactor) + r*alphaFactor, info.g*(1.0-alphaFactor) + g*alphaFactor, info.b*(1.0-alphaFactor) + b*alphaFactor, a);
				}
			}
		}

		// In case of OpenGL backend, SDLImage needs to be converted
		Image* img = m_renderbackend->createImage(sts.str(), overlay_surface);

		if (found) {
			// image exists but is not "loaded"
			removeFromCheck(info.overlay);
			ImagePtr temp(img);
			info.overlay.get()->copySubimage(0, 0, temp);
			info.overlay.get()->setState(IResource::RES_LOADED);
		} else {
			// add image
			img->setState(IResource::RES_LOADED);
			info.overlay = ImageManager::instance()->add(img);
		}
		// mark overlay as not dirty since we created/recreated it here
		info.dirty = false;

		return info.overlay.get();
	}

	ImagePtr InstanceRenderer::getMultiColorOverlay(const RenderItem& vc, OverlayColors* colors) {
		// multi color overlay
		const std::map<Color, Color>& colorMap = colors ? colors->getColors() : vc.getColorOverlay()->getColors();
		std::map<Color, Color>::const_iterator it = colorMap.begin();
		ImagePtr colorOverlayImage = colors ? colors->getColorOverlayImage() : vc.getColorOverlay()->getColorOverlayImage();
		ImagePtr colorOverlay;

		// create name
		std::stringstream sts;
		sts << colorOverlayImage.get()->getName();
		for (; it != colorMap.end(); ++it) {
			sts << "," << static_cast<uint32_t>(it->second.getR() | (it->second.getG() << 8) | (it->second.getB() << 16) | (it->second.getAlpha()<<24));
		}
		bool exist = false;
		bool found = false;
		if (ImageManager::instance()->exists(sts.str())) {
			exist = true;
			colorOverlay = ImageManager::instance()->getPtr(sts.str());
			if (isValidImage(colorOverlay)) {
				removeFromCheck(colorOverlay);
				found = true;
			}
		}
		if (!exist || !found) {
			// With lazy loading we can come upon a situation where we need to generate color overlay from
			// uninitialised shared image
			if (colorOverlayImage->isSharedImage()) {
				colorOverlayImage->forceLoadInternal();
			}

			// not found so we create it
			SDL_Surface* overlay_surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA,
				colorOverlayImage->getWidth(), colorOverlayImage->getHeight(), 32,
				RMASK, GMASK, BMASK, AMASK);

			uint8_t r, g, b, a = 0;
			for (int32_t x = 0; x < overlay_surface->w; x++) {
				for (int32_t y = 0; y < overlay_surface->h; y++) {
					colorOverlayImage->getPixelRGBA(x, y, &r, &g, &b, &a);
					Color c(r, g, b, a);
					it = colorMap.find(c);
					if (it != colorMap.end()) {
						Image::putPixel(overlay_surface, x, y, it->second.getR(), it->second.getG(), it->second.getB(), it->second.getAlpha());
					} else {
						Image::putPixel(overlay_surface, x, y, r, g, b, a);
					}
				}
			}

			// In case of OpenGL backend, SDLImage needs to be converted
			Image* img = m_renderbackend->createImage(sts.str(), overlay_surface);

			if (exist) {
				// image exists but is not "loaded"
				removeFromCheck(colorOverlay);
				ImagePtr temp(img);
				colorOverlay.get()->setSurface(img->detachSurface());
				colorOverlay.get()->setState(IResource::RES_LOADED);
			} else {
				// add image
				img->setState(IResource::RES_LOADED);
				colorOverlay = ImageManager::instance()->add(img);
			}
		}
		addToCheck(colorOverlay);
		return colorOverlay;
	}

	void InstanceRenderer::addOutlined(Instance* instance, int32_t r, int32_t g, int32_t b, int32_t width, int32_t threshold) {
		OutlineInfo newinfo(this);
		newinfo.r = r;
		newinfo.g = g;
		newinfo.b = b;
		newinfo.threshold = threshold;
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
				info.threshold = threshold;
				info.dirty = true;
			}
		} else {
			std::pair<InstanceToEffects_t::iterator, bool> iter = m_assigned_instances.insert(std::make_pair(instance, OUTLINE));
			if (iter.second) {
				instance->addDeleteListener(m_delete_listener);
			} else {
				Effect& effect = iter.first->second;
				if ((effect & OUTLINE) != OUTLINE) {
					effect += OUTLINE;
				}
			}
		}
	}

	void InstanceRenderer::addColored(Instance* instance, int32_t r, int32_t g, int32_t b, int32_t a) {
		ColoringInfo newinfo(this);
		newinfo.r = r;
		newinfo.g = g;
		newinfo.b = b;
		newinfo.a = a;
		newinfo.dirty = true;

		// attempts to insert the element into the coloring map
		// will return false in the second value of the pair if the instance already exists
		// in the map and the first value of the pair will then be an iterator to the
		// existing data for the instance
		std::pair<InstanceToColoring_t::iterator, bool> insertiter = m_instance_colorings.insert(std::make_pair(instance, newinfo));

		if (insertiter.second == false) {
			// the insertion did not happen because the instance
			// already exists in the map so lets just update its coloring info
			ColoringInfo& info = insertiter.first->second;

			if (info.r != r || info.g != g || info.b != b || info.a != a) {
				// only update the coloring info if its changed since the last call
				info.r = r;
				info.b = b;
				info.g = g;
				info.a = a;
				info.dirty = true;
			}
		} else {
			std::pair<InstanceToEffects_t::iterator, bool> iter = m_assigned_instances.insert(std::make_pair(instance, COLOR));
			if (iter.second) {
				instance->addDeleteListener(m_delete_listener);
			} else {
				Effect& effect = iter.first->second;
				if ((effect & COLOR) != COLOR) {
					effect += COLOR;
				}
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
			info.groups = groups;
			info.w = w;
			info.h = h;
			info.trans = trans;
			info.front = front;
		} else {
			std::pair<InstanceToEffects_t::iterator, bool> iter = m_assigned_instances.insert(std::make_pair(instance, AREA));
			if (iter.second) {
				instance->addDeleteListener(m_delete_listener);
			} else {
				Effect& effect = iter.first->second;
				if ((effect & AREA) != AREA) {
					effect += AREA;
				}
			}
		}
	}

	void InstanceRenderer::removeOutlined(Instance* instance) {
		InstanceToEffects_t::iterator it = m_assigned_instances.find(instance);
		if (it != m_assigned_instances.end()) {
			if (it->second == OUTLINE) {
				instance->removeDeleteListener(m_delete_listener);
				m_instance_outlines.erase(instance);
				m_assigned_instances.erase(it);
			} else if ((it->second & OUTLINE) == OUTLINE) {
				it->second -= OUTLINE;
				m_instance_outlines.erase(instance);
			}
		}
	}

	void InstanceRenderer::removeColored(Instance* instance) {
		InstanceToEffects_t::iterator it = m_assigned_instances.find(instance);
		if (it != m_assigned_instances.end()) {
			if (it->second == COLOR) {
				instance->removeDeleteListener(m_delete_listener);
				m_instance_colorings.erase(instance);
				m_assigned_instances.erase(it);
			} else if ((it->second & COLOR) == COLOR) {
				it->second -= COLOR;
				m_instance_colorings.erase(instance);
			}
		}
	}

	void InstanceRenderer::removeTransparentArea(Instance* instance) {
		InstanceToEffects_t::iterator it = m_assigned_instances.find(instance);
		if (it != m_assigned_instances.end()) {
			if (it->second == AREA) {
				instance->removeDeleteListener(m_delete_listener);
				m_instance_areas.erase(instance);
				m_assigned_instances.erase(it);
			} else if ((it->second & AREA) == AREA) {
				it->second -= AREA;
				m_instance_areas.erase(instance);
			}
		}
	}

	void InstanceRenderer::removeAllOutlines() {
		if (!m_instance_outlines.empty()) {
			InstanceToOutlines_t::iterator outline_it = m_instance_outlines.begin();
			for (; outline_it != m_instance_outlines.end(); ++outline_it) {
				InstanceToEffects_t::iterator it = m_assigned_instances.find((*outline_it).first);
				if (it != m_assigned_instances.end()) {
					if (it->second == OUTLINE) {
						(*outline_it).first->removeDeleteListener(m_delete_listener);
						m_assigned_instances.erase(it);
					} else if ((it->second & OUTLINE) == OUTLINE) {
						it->second -= OUTLINE;
					}
				}
			}
			m_instance_outlines.clear();
		}
	}

	void InstanceRenderer::removeAllColored() {
		if (!m_instance_colorings.empty()) {
			InstanceToColoring_t::iterator color_it = m_instance_colorings.begin();
			for (; color_it != m_instance_colorings.end(); ++color_it) {
				InstanceToEffects_t::iterator it = m_assigned_instances.find((*color_it).first);
				if (it != m_assigned_instances.end()) {
					if (it->second == COLOR) {
						(*color_it).first->removeDeleteListener(m_delete_listener);
						m_assigned_instances.erase(it);
					} else if ((it->second & COLOR) == COLOR) {
						it->second -= COLOR;
					}
				}
			}
			m_instance_colorings.clear();
		}
	}

	void InstanceRenderer::removeAllTransparentAreas() {
		if (!m_instance_areas.empty()) {
			InstanceToAreas_t::iterator area_it = m_instance_areas.begin();
			for (; area_it != m_instance_areas.end(); ++area_it) {
				InstanceToEffects_t::iterator it = m_assigned_instances.find((*area_it).first);
				if (it != m_assigned_instances.end()) {
					if (it->second == AREA) {
						(*area_it).first->removeDeleteListener(m_delete_listener);
						m_assigned_instances.erase(it);
					} else if ((it->second & AREA) == AREA) {
						it->second -= AREA;
					}
				}
			}
			m_instance_areas.clear();
		}
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
		// stop timer
		if (m_timer_enabled) {
			m_timer.stop();
		}
		// remove all effects and listener
		removeAllOutlines();
		removeAllColored();
		removeAllTransparentAreas();
		removeAllIgnoreLight();
		// removes the references to the effect images
		m_check_images.clear();
	}

	void InstanceRenderer::setRemoveInterval(uint32_t interval) {
		if (m_interval != interval*1000) {
			m_interval = interval*1000;
			m_timer.setInterval(m_interval);
		}
	}

	uint32_t InstanceRenderer::getRemoveInterval() const {
		return m_interval/1000;
	}

	void InstanceRenderer::addToCheck(const ImagePtr& image) {
		if (isValidImage(image)) {
			// if image is already inserted then return
			ImagesToCheck_t::iterator it = m_check_images.begin();
			for (; it != m_check_images.end(); ++it) {
				if (it->image.get()->getName() == image.get()->getName()) {
					return;
				}
			}
			s_image_entry entry;
			entry.image = image;
			entry.timestamp = TimeManager::instance()->getTime();
			m_check_images.push_front(entry);

			if (!m_timer_enabled) {
				m_timer_enabled = true;
				m_timer.start();
			}
		}
	}

	void InstanceRenderer::check() {
		uint32_t now = TimeManager::instance()->getTime();
		ImagesToCheck_t::iterator it = m_check_images.begin();
		// free unused images
		while (it != m_check_images.end()) {
			if (now - it->timestamp > m_interval) {
				if (isValidImage(it->image)) {
					ImageManager::instance()->free(it->image.get()->getName());
				}
				it = m_check_images.erase(it);
			} else {
				++it;
			}
		}

		if (m_check_images.empty() && m_timer_enabled) {
			m_timer_enabled = false;
			m_timer.stop();
		}
	}

	void InstanceRenderer::removeFromCheck(const ImagePtr& image) {
		if (isValidImage(image)) {
			// if the image is used then remove it here
			ImagesToCheck_t::iterator it = m_check_images.begin();
			for (; it != m_check_images.end(); ++it) {
				if (it->image.get()->getName() == image.get()->getName()) {
					m_check_images.erase(it);
					break;
				}
			}

			if (m_check_images.empty() && m_timer_enabled) {
				m_timer_enabled = false;
				m_timer.stop();
			}
		}
	}

	void InstanceRenderer::removeInstance(Instance* instance) {
		InstanceToEffects_t::iterator it = m_assigned_instances.find(instance);
		if (it != m_assigned_instances.end()) {
			m_instance_outlines.erase(instance);
			m_instance_colorings.erase(instance);
			m_instance_areas.erase(instance);
			instance->removeDeleteListener(m_delete_listener);
			m_assigned_instances.erase(it);
		}
	}

	bool InstanceRenderer::isValidImage(const ImagePtr& image) {
		if (image.get()) {
			if (image.get()->getState() == IResource::RES_LOADED) {
				return true;
			}
		}
		return false;
	}
}
