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
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/action.h"
#include "model/metamodel/timeprovider.h"
#include "model/structures/map.h"
#include "model/structures/layer.h"
#include "model/structures/instancetree.h"
#include "model/structures/instance.h"
#include "model/structures/location.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"
#include "util/time/timemanager.h"
#include "video/renderbackend.h"
#include "video/image.h"
#include "video/imagepool.h"
#include "video/animation.h"
#include "video/animationpool.h"

#include "camera.h"
#include "visual.h"


namespace FIFE {
	static Logger _log(LM_CAMERA);

	Camera::Camera(const std::string& id, 
		Layer *layer, 
		Rect viewport, 
		ExactModelCoordinate emc,
		RenderBackend* renderbackend,
		ImagePool* ipool,
		AnimationPool* apool):
			m_id(id),
			m_matrix(),
			m_inverse_matrix(),
			m_tilt(0),
			m_rotation(0),
			m_zoom(1),
			m_location(),
			m_prev_origo(ScreenPoint(0,0,0)),
			m_cur_origo(ScreenPoint(0,0,0)),
			m_viewport(),
			m_screen_cell_width(1),
			m_screen_cell_height(1),
			m_reference_scale(1),
			m_enabled(true),
			m_attachedto(NULL),
			m_image_dimensions(),
			m_iswarped(false),
			m_renderers(),
			m_pipeline(),
			m_updated(false),
			m_renderbackend(renderbackend),
			m_ipool(ipool),
			m_apool(apool),
			m_layer_to_instances() {
		m_location.setLayer(layer);
		m_location.setExactLayerCoordinates(emc);
		m_viewport = viewport;
	}

	Camera::~Camera() {
		std::map<std::string, RendererBase*>::iterator r_it = m_renderers.begin();
		for(; r_it != m_renderers.end(); ++r_it) {
			delete r_it->second;
		}
		m_renderers.clear();
	}

	void Camera::setTilt(double tilt) {
		m_tilt = tilt;
		updateReferenceScale();
		updateMatrices();
		m_iswarped = true;
	}

	double Camera::getTilt() const {
		return m_tilt;
	}

	void Camera::setRotation(double rotation) {
		m_rotation = rotation;
		updateReferenceScale();
		updateMatrices();
		m_iswarped = true;
	}

	double Camera::getRotation() const {
		return m_rotation;
	}

	void Camera::setZoom(double zoom) {
		m_zoom = zoom;
		if (m_zoom < 0.001) {
			m_zoom = 0.001;
		}
		updateMatrices();
		m_iswarped = true;
	}

	double Camera::getZoom() const {
		return m_zoom;
	}

	void Camera::setCellImageDimensions(unsigned int width, unsigned int height) {
		m_screen_cell_width = width;
		m_screen_cell_height = height;
		updateReferenceScale();
		updateMatrices();
		m_iswarped = true;
	}

	void Camera::setLocation(const Location& location) {
		// initialize first set properly
		if ((m_prev_origo == m_cur_origo) && (m_prev_origo == ScreenPoint(0,0,0))) {
			m_cur_origo = toScreenCoordinates(ExactModelCoordinate(0,0,0));
			m_prev_origo = m_cur_origo;
		}
		m_location = location;
		
		CellGrid* cg = NULL;
		if (m_location.getLayer()) {
			cg = m_location.getLayer()->getCellGrid();
		} else {
			throw Exception("Location without layer given to Camera::setLocation");
		}
		if (!cg) {
			throw Exception("Camera layer has no cellgrid specified");
		}
		
		updateMatrices();
		
		m_cur_origo = toScreenCoordinates(ExactModelCoordinate(0,0,0));
	}

	Point Camera::getCellImageDimensions() {
		return getCellImageDimensions(m_location.getLayer());
	}

	Point Camera::getCellImageDimensions(Layer* layer) {
		if (layer == m_location.getLayer()) {
			return Point( m_screen_cell_width, m_screen_cell_height );
		}
		std::map<Layer*, Point>::iterator it = m_image_dimensions.find(layer);
		if (it != m_image_dimensions.end()) {
			return it->second;
		}
		Point p;
		CellGrid* cg = layer->getCellGrid();
		assert(cg);
		DoublePoint dimensions = getLogicalCellDimensions(layer);
		p.x = static_cast<int>(round(m_reference_scale * dimensions.x));
		p.y = static_cast<int>(round(m_reference_scale * dimensions.y));
		m_image_dimensions[layer] = p;
		return p;
	}
	
	Location Camera::getLocation() const {
		return m_location;
	}

	Location& Camera::getLocationRef() {
		return m_location;
	}
	
	void Camera::setViewPort(const Rect& viewport) {
		m_viewport = viewport;
	}

	const Rect& Camera::getViewPort() const {
		return m_viewport;
	}

	void Camera::setEnabled(bool enabled) {
		m_enabled = enabled;
	}
	
	bool Camera::isEnabled() {
		return m_enabled;
	}

	void Camera::updateMatrices() {
		double scale = m_reference_scale;
		m_matrix.loadScale(scale, scale, scale);
		if (m_location.getLayer()) {
			CellGrid* cg = m_location.getLayer()->getCellGrid();
			if (cg) {
				ExactModelCoordinate pt = m_location.getMapCoordinates();
				m_matrix.applyTranslate( -pt.x *m_reference_scale,-pt.y *m_reference_scale, 0);
			}
		}
		scale = m_zoom;
		m_matrix.applyScale(scale, scale, scale);
		m_matrix.applyRotate(-m_rotation, 0.0, 0.0, 1.0);
		m_matrix.applyRotate(-m_tilt, 1.0, 0.0, 0.0);
		m_inverse_matrix = m_matrix.inverse();
	}

	void Camera::calculateZValue(ScreenPoint& screen_coords) {
		int dy = -(screen_coords.y - toScreenCoordinates(m_location.getMapCoordinates()).y);
		screen_coords.z = static_cast<int>(tan(m_tilt * (M_PI / 180.0)) * static_cast<double>(dy));
	}
	
	ExactModelCoordinate Camera::toMapCoordinates(ScreenPoint screen_coords, bool z_calculated) {
		if (!z_calculated) {
			calculateZValue(screen_coords);
		}
		screen_coords.x -= m_viewport.w / 2;
		screen_coords.y -= m_viewport.h / 2;

		return m_inverse_matrix  * intPt2doublePt(screen_coords);
	}

	ScreenPoint Camera::toScreenCoordinates(ExactModelCoordinate elevation_coords) {
		ExactModelCoordinate p = elevation_coords;
		ScreenPoint pt = doublePt2intPt( m_matrix* p  );
		pt.x += m_viewport.w / 2;
		pt.y += m_viewport.h / 2;
		return pt;
	}

	DoublePoint Camera::getLogicalCellDimensions(Layer* layer) {
		CellGrid* cg = NULL;
		if (layer) {
			cg = layer->getCellGrid();
		}
		assert(cg);

		ModelCoordinate cell(0,0);
		std::vector<ExactModelCoordinate> vertices;
		cg->getVertices(vertices, cell);

		DoubleMatrix mtx;
		mtx.loadRotate(m_rotation, 0.0, 0.0, 1.0);
		mtx.applyRotate(m_tilt, 1.0, 0.0, 0.0);
		double x1, x2, y1, y2;
		for (unsigned int i = 0; i < vertices.size(); i++) {
			vertices[i] = cg->toMapCoordinates(vertices[i]);
			vertices[i] = mtx * vertices[i];
			if (i == 0) {
				x1 = x2 = vertices[0].x;
				y1 = y2 = vertices[0].y;
			} else {
				x1 = std::min(vertices[i].x, x1);
				x2 = std::max(vertices[i].x, x2);
				y1 = std::min(vertices[i].y, y1);
				y2 = std::max(vertices[i].y, y2);
			}
		}
		return DoublePoint( x2 - x1, y2 - y1 );
	}

	void Camera::updateReferenceScale() {
		DoublePoint dim = getLogicalCellDimensions(m_location.getLayer());
		m_reference_scale = static_cast<double>(m_screen_cell_width) / dim.x;

		FL_DBG(_log, "Updating reference scale");
		FL_DBG(_log, LMsg("   tilt=") << m_tilt << " rot=" << m_rotation);
		FL_DBG(_log, LMsg("   m_screen_cell_width=") << m_screen_cell_width);
	}
	
	void Camera::getMatchingInstances(ScreenPoint screen_coords, Layer& layer, std::list<Instance*>& instances) {
		instances.clear();
		const std::vector<Instance*>& layer_instances = m_layer_to_instances[&layer];
		std::vector<Instance*>::const_iterator instance_it = layer_instances.end();
		while (instance_it != layer_instances.begin()) {
			--instance_it;
			Instance* i = (*instance_it);
			InstanceVisual* visual = i->getVisual<InstanceVisual>();
			InstanceVisualCacheItem& vc = visual->getCacheItem(this);
			if ((vc.dimensions.contains(Point(screen_coords.x, screen_coords.y)))) {
				assert(vc.image);
				Uint8 r, g, b, a;
				int x = screen_coords.x - vc.dimensions.x;
				int y = screen_coords.y - vc.dimensions.y;
				if (m_zoom != 1.0) {
					double fx = static_cast<double>(x);
					double fy = static_cast<double>(y);
					double fow = static_cast<double>(vc.image->getWidth());
					double foh = static_cast<double>(vc.image->getHeight());
					double fsw = static_cast<double>(vc.dimensions.w);
					double fsh = static_cast<double>(vc.dimensions.h);
					x = static_cast<int>(round(fx / fsw * fow));
					y = static_cast<int>(round(fy / fsh * foh));
				}
				vc.image->getPixelRGBA(x, y, &r, &g, &b, &a);
				// instance is hit with mouse if not totally transparent
				if (a != 0) {
					instances.push_back(i);
				}
			}
		}
	}
	
	void Camera::getMatchingInstances(Rect screen_rect, Layer& layer, std::list<Instance*>& instances) {
		instances.clear();
		const std::vector<Instance*>& layer_instances = m_layer_to_instances[&layer];
		std::vector<Instance*>::const_iterator instance_it = layer_instances.end();
		while (instance_it != layer_instances.begin()) {
			--instance_it;
			Instance* i = (*instance_it);
			InstanceVisual* visual = i->getVisual<InstanceVisual>();
			InstanceVisualCacheItem& vc = visual->getCacheItem(this);
			if ((vc.dimensions.intersects(screen_rect))) {
				assert(vc.image);
				Uint8 r, g, b, a;
				for(int xx = screen_rect.x; xx < screen_rect.x + screen_rect.w; xx++) {
					for(int yy = screen_rect.y; yy < screen_rect.y + screen_rect.h; yy++) {
						if ((vc.dimensions.contains(Point(xx, yy)))) {
							int x = xx - vc.dimensions.x;
							int y = yy - vc.dimensions.y;
							if (m_zoom != 1.0) {
								double fx = static_cast<double>(x);
								double fy = static_cast<double>(y);
								double fow = static_cast<double>(vc.image->getWidth());
								double foh = static_cast<double>(vc.image->getHeight());
								double fsw = static_cast<double>(vc.dimensions.w);
								double fsh = static_cast<double>(vc.dimensions.h);
								x = static_cast<int>(round(fx / fsw * fow));
								y = static_cast<int>(round(fy / fsh * foh));
							}
							vc.image->getPixelRGBA(x, y, &r, &g, &b, &a);
							// instance is hit with mouse if not totally transparent
							if (a != 0) {
								instances.push_back(i);
								goto found_non_transparent_pixel;
							}
						}
					}
				}
				found_non_transparent_pixel:;
			}
		}
	}
	
	void Camera::getMatchingInstances(Location& loc, std::list<Instance*>& instances, bool use_exactcoordinates) {
		instances.clear();
		const std::vector<Instance*>& layer_instances = m_layer_to_instances[loc.getLayer()];
		std::vector<Instance*>::const_iterator instance_it = layer_instances.end();
		while (instance_it != layer_instances.begin()) {
			--instance_it;
			Instance* i = (*instance_it);
			if (use_exactcoordinates) {
				if (i->getLocationRef().getExactLayerCoordinatesRef() == loc.getExactLayerCoordinatesRef()) {
					instances.push_back(i);
				}
			} else {
				if (i->getLocationRef().getLayerCoordinates() == loc.getLayerCoordinates()) {
					instances.push_back(i);
				}
			}
		}
	}

	void Camera::attach(Instance *instance) {
		// fail if the layers aren't the same
		if (m_location.getLayer()->getId() != instance->getLocation().getLayer()->getId()) {
			FL_WARN(_log, "Tried to attach camera to instance on different layer.");
			return ;
		}
		m_attachedto = instance;
	}

	void Camera::detach() {
		m_attachedto = NULL;
	}

	void Camera::update() {
		if( !m_attachedto ) {
			return;
		}
		Location loc(m_location);
		loc.setExactLayerCoordinates( m_attachedto->getLocationRef().getExactLayerCoordinates(m_location.getLayer()) );
		setLocation(loc);
		updateMatrices();
	}
	
	void Camera::refresh() {
		updateMatrices();
		m_iswarped = true;
	}
	
	void Camera::resetUpdates() {
		m_iswarped = false;
		m_prev_origo = m_cur_origo;
	}
	
	bool pipelineSort(const RendererBase* lhs, const RendererBase* rhs) {
		return (lhs->getPipelinePosition() < rhs->getPipelinePosition());
	}
	
	void Camera::addRenderer(RendererBase* renderer) {
		renderer->setRendererListener(this);
		m_renderers[renderer->getName()] = renderer;
		if (renderer->isEnabled()) {
			m_pipeline.push_back(renderer);
		}
		m_pipeline.sort(pipelineSort);
	}
	
	void Camera::onRendererPipelinePositionChanged(RendererBase* renderer) {
		m_pipeline.sort(pipelineSort);
	}
	
	void Camera::onRendererEnabledChanged(RendererBase* renderer) {
		assert(m_renderers[renderer->getName()]);
		if (renderer->isEnabled()) {
			FL_LOG(_log, LMsg("Enabling renderer ") << renderer->getName());
			m_pipeline.push_back(renderer);
			m_pipeline.sort(pipelineSort);
		} else {
			m_pipeline.remove(renderer);
		}
	}

	RendererBase* Camera::getRenderer(const std::string& name) {
		return m_renderers[name];
	}
	
	class InstanceDistanceSort {
	public:
		Camera* cam;
		inline bool operator()(const Instance* lhs, const Instance* rhs) {
			InstanceVisual* liv = lhs->getVisual<InstanceVisual>();
			InstanceVisual* riv = rhs->getVisual<InstanceVisual>();
			InstanceVisualCacheItem& lic = liv->getCacheItem(cam);
			InstanceVisualCacheItem& ric = riv->getCacheItem(cam);
			if (lic.screenpoint.z == ric.screenpoint.z) {
				return liv->getStackPosition() < riv->getStackPosition();
			}
			return lic.screenpoint.z < ric.screenpoint.z;
		}
	};

	void Camera::resetRenderers() {
		std::map<std::string, RendererBase*>::iterator r_it = m_renderers.begin();
		for (; r_it != m_renderers.end(); ++r_it) {
			Map* map = m_location.getMap();
			r_it->second->reset();
			r_it->second->activateAllLayers(map);
		}
	}

	void Camera::render() {
		ScreenPoint cammove = m_prev_origo - m_cur_origo;
		
		Map* map = m_location.getMap();
		if (!map) {
			FL_ERR(_log, "No map for camera found");
			return;
		}
		//if ((!map->isChanged()) && (!m_iswarped) && (cammove == ScreenPoint(0,0,0))) {
		//	return;
		//}
		
		// update each layer
		m_renderbackend->pushClipArea(getViewPort());
		
		m_layer_to_instances.clear();

		const std::list<Layer*>& layers = map->getLayers();
		std::list<Layer*>::const_iterator layer_it = layers.begin();
		for (;layer_it != layers.end(); ++layer_it) {

			// sort instances on layer based on stack position + camera distance. done only once
			//  here instead passing it to each renderer.
			// instances are checked first if their image intersects with the viewport.
			// this reduces processing load during sorting later
			const std::vector<Instance*>& allinstances = (*layer_it)->getInstances();
			std::vector<Instance*>::const_iterator instance_it = allinstances.begin();
			std::vector<Instance*>& instances_to_render = m_layer_to_instances[*layer_it];
			for (;instance_it != allinstances.end(); ++instance_it) {
				Instance* instance = *instance_it;
				InstanceVisual* visual = instance->getVisual<InstanceVisual>();
				if(!visual->isVisible())
					continue;
				InstanceVisualCacheItem& vc = visual->getCacheItem(this);
				
				// use cached values if there is no need to do full recalculation
				ScreenPoint drawpt;
				int angle = 0;
				if (m_updated && (!m_iswarped) && (!(instance->getChangeInfo() & (ICHANGE_LOC | ICHANGE_ROTATION | ICHANGE_FACING_LOC))) && (vc.image)) {
					int pos_estimate_x = vc.screenpoint.x - cammove.x;
					int pos_estimate_y = vc.screenpoint.y - cammove.y;
					int pos_estimate_z = vc.screenpoint.z - cammove.z;
					angle = vc.facing_angle;
					//std::cout << "orig x = " << drawpt.x << ", est x = " << pos_estimate_x << "\n";
					//std::cout << "orig y = " << drawpt.y << ", est y = " << pos_estimate_y << "\n";
					drawpt.x = pos_estimate_x;
					drawpt.y = pos_estimate_y;
					drawpt.z = pos_estimate_z;
					//drawpt.z = toScreenCoordinates( instance->getLocationRef().getMapCoordinates() ).z;
				} else {
					drawpt = toScreenCoordinates( instance->getLocationRef().getMapCoordinates() );
					vc.facing_angle = angle = getAngleBetween(instance->getLocationRef(), instance->getFacingLocation());
				}
				angle += instance->getRotation();

				Image* image = NULL;
				Action* action = instance->getCurrentAction();
				if (action) {
					FL_DBG(_log, "Instance has action");
					int animation_id = action->getVisual<ActionVisual>()->getAnimationIndexByAngle(angle);
					Animation& animation = m_apool->getAnimation(animation_id);
					unsigned int animtime = instance->getActionRuntime() % animation.getDuration();
					image = animation.getFrameByTimestamp(animtime);
				} else {
					FL_DBG(_log, "No action");
					int imageid = vc.getStaticImageIndexByAngle(angle, instance);
					FL_DBG(_log, LMsg("Instance does not have action, using static image with id ") << imageid);
					if (imageid >= 0) {
						image = &m_ipool->getImage(imageid);
					} else {
						// there was no static image for instance, trying default action
						action = instance->getObject()->getDefaultAction();
						if (action) {
							int animation_id = action->getVisual<ActionVisual>()->getAnimationIndexByAngle(angle);
							Animation& animation = m_apool->getAnimation(animation_id);
							unsigned int animtime = instance->getActionRuntime() % animation.getDuration();
							image = animation.getFrameByTimestamp(animtime);
						}
					}
				}
				if (image) {
					vc.image = image;
					vc.screenpoint = drawpt;
					
					int w = image->getWidth();
					int h = image->getHeight();
					drawpt.x -= w / 2;
					drawpt.x += image->getXShift();
					drawpt.y -= h / 2;
					drawpt.y += image->getYShift();
					Rect r = Rect(drawpt.x, drawpt.y, w, h);
					
					vc.dimensions = r;
					if (m_zoom != 1.0) {
						// NOTE: Due to image alignment, there is additional additions and substractions on image dimensions
						//       There's probabaly some better solution for this, but works "good enough" for now.
						//       In case additions / substractions are removed, gaps appear between tiles.
						r.w = static_cast<unsigned int>(ceil(static_cast<double>(vc.dimensions.w) * m_zoom)) + 2;
						r.h = static_cast<unsigned int>(ceil(static_cast<double>(vc.dimensions.h) * m_zoom)) + 2;
						int xo = static_cast<int>(ceil(static_cast<double>(vc.image->getXShift()) * m_zoom)) - vc.image->getXShift();
						int yo = static_cast<int>(ceil(static_cast<double>(vc.image->getYShift()) * m_zoom)) - vc.image->getYShift();
						r.x = vc.dimensions.x - static_cast<unsigned int>(ceil(static_cast<double>(r.w - vc.dimensions.w) / 2)) + xo - 1;
						r.y = vc.dimensions.y - static_cast<unsigned int>(ceil(static_cast<double>(r.h - vc.dimensions.h) / 2)) + yo - 1;
						vc.dimensions = r;
					}
					
					if (vc.dimensions.intersects(getViewPort())) {
						instances_to_render.push_back(instance);
					}
				}
			}

			InstanceDistanceSort ids;
			ids.cam = this;
			std::stable_sort(instances_to_render.begin(), instances_to_render.end(), ids);

			std::list<RendererBase*>::iterator r_it = m_pipeline.begin();
			for (; r_it != m_pipeline.end(); ++r_it) {
				if ((*r_it)->isActivedLayer(*layer_it)) {
					(*r_it)->render(this, *layer_it, instances_to_render);
				}
			}
		}
		m_renderbackend->popClipArea();
		resetUpdates();
		m_updated = true;
	}

}
