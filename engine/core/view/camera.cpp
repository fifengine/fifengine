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
#include "util/math/angles.h"
#include "util/time/timemanager.h"
#include "video/renderbackend.h"
#include "video/image.h"
#include "video/animation.h"
#include "video/imagemanager.h"

#include "camera.h"
#include "layercache.h"
#include "visual.h"


namespace FIFE {
	static Logger _log(LM_CAMERA);

	class MapObserver : public MapChangeListener {
		Camera* m_camera;

	public:
		MapObserver(Camera* camera) {
			m_camera = camera;
		}
		virtual ~MapObserver() {}

		virtual void onMapChanged(Map* map, std::vector<Layer*>& changedLayers) {
		}

		virtual void onLayerCreate(Map* map, Layer* layer) {
			m_camera->addLayer(layer);
		}

		virtual void onLayerDelete(Map* map, Layer* layer) {
			m_camera->removeLayer(layer);
		}
	};

	Camera::Camera(const std::string& id,
		Layer *layer,
		const Rect& viewport,
		RenderBackend* renderbackend):
			m_id(id),
			m_matrix(),
			m_inverse_matrix(),
			m_tilt(0),
			m_rotation(0),
			m_zoom(1),
			m_location(),
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
			m_layer_to_instances(),
			m_lighting(false),
			m_light_colors(),
			m_col_overlay(false),
			m_img_overlay(false),
			m_ani_overlay(false) {
		m_viewport = viewport;
		m_map_observer = new MapObserver(this);
		m_map = 0;
		Location location;
		location.setLayer(layer);
		setLocation(location);
	}

	Camera::~Camera() {
		// Trigger removal of LayerCaches and MapObserver
		updateMap(NULL);

		std::map<std::string, RendererBase*>::iterator r_it = m_renderers.begin();
		for(; r_it != m_renderers.end(); ++r_it) {
			r_it->second->reset();
			delete r_it->second;
		}
		m_renderers.clear();
		delete m_map_observer;
	}

	void Camera::setTilt(double tilt) {
		if (!Mathd::Equal(m_tilt, tilt)) {
			m_tilt = tilt;
			updateReferenceScale();
			updateMatrices();
			m_iswarped = true;
		}
	}

	double Camera::getTilt() const {
		return m_tilt;
	}

	void Camera::setRotation(double rotation) {
		if (!Mathd::Equal(m_rotation, rotation)) {
			m_rotation = rotation;
			updateReferenceScale();
			updateMatrices();
			m_iswarped = true;
		}
	}

	double Camera::getRotation() const {
		return m_rotation;
	}

	void Camera::setZoom(double zoom) {
		if (!Mathd::Equal(m_zoom, zoom)) {
			m_zoom = zoom;
			if (m_zoom < 0.001) {
				m_zoom = 0.001;
			}
			updateMatrices();
			m_updated = false;
		}
	}

	double Camera::getZoom() const {
		return m_zoom;
	}

	void Camera::setCellImageDimensions(uint32_t width, uint32_t height) {
		m_screen_cell_width = width;
		m_screen_cell_height = height;
		updateReferenceScale();
		updateMatrices();
		m_iswarped = true;
	}

	void Camera::setLocation(const Location& location) {
		if (m_location == location ) {
			return;
		}

		CellGrid* cell_grid = NULL;
		if (location.getLayer()) {
			cell_grid = location.getLayer()->getCellGrid();
		} else {
			throw Exception("Location without layer given to Camera::setLocation");
		}
		if (!cell_grid) {
			throw Exception("Camera layer has no cellgrid specified");
		}

		m_location = location;
		updateMatrices();

		ExactModelCoordinate emc = m_location.getMapCoordinates();
		m_cur_origo = toScreenCoordinates(emc);

		// WARNING
		// It is important that m_location is already set,
		// as the updates which are triggered here
		// need to calculate screen-coordinates
		// which depend on m_location.
		updateMap(m_location.getMap());

		m_updated = false;
	}

	void Camera::updateMap(Map* map) {
		if(m_map == map) {
			return;
		}
		if(m_map) {
			m_map->removeChangeListener(m_map_observer);
			const std::list<Layer*>& layers = m_map->getLayers();
			for(std::list<Layer*>::const_iterator i = layers.begin(); i !=layers.end(); ++i) {
				removeLayer(*i);
			}
		}
		if(map) {
			map->addChangeListener(m_map_observer);
			const std::list<Layer*>& layers = map->getLayers();
			for(std::list<Layer*>::const_iterator i = layers.begin(); i !=layers.end(); ++i) {
				addLayer(*i);
			}
		}
		m_map = map;
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
		p.x = static_cast<int32_t>(round(m_reference_scale * dimensions.x));
		p.y = static_cast<int32_t>(round(m_reference_scale * dimensions.y));
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

	Point3D Camera::getOrigin() const {
		return m_cur_origo;
	}

	void Camera::updateMatrices() {
		double scale = m_reference_scale;
		m_matrix.loadScale(scale, scale, scale);
		m_vs_matrix.loadScale(scale,scale,scale);
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
		m_matrix.applyTranslate(+m_viewport.x+m_viewport.w/2, +m_viewport.y+m_viewport.h/2, 0);
		m_inverse_matrix = m_matrix.inverse();

		m_vs_matrix.applyRotate(-m_rotation, 0.0, 0.0, 1.0);
		m_vs_matrix.applyRotate(-m_tilt, 1.0, 0.0, 0.0);
		m_vs_inverse_matrix = m_vs_matrix.inverse();

		// calculate the screen<->virtual screen transformation
		// this explicitly ignores the z-value.
		m_vscreen_2_screen = m_matrix;
		// NOTE: mult4by4 is an in-place modification.
		m_vscreen_2_screen.mult4by4(m_vs_inverse_matrix);
		// set the z transformation to unity
		const int32_t N=4;
		for(int32_t i=0; i!=N; ++i) {
			  m_vscreen_2_screen[2*N + i] = 0;
			  m_vscreen_2_screen[i*N + 2] = 0;
		}
		m_vscreen_2_screen[2*N + 2] = 1;
		m_screen_2_vscreen = m_vscreen_2_screen.inverse();

		// FL_WARN(_log, LMsg("matrix: ") << m_matrix << " 1: " << m_matrix.inverse().mult4by4(m_matrix));
// 		FL_WARN(_log, LMsg("vs2s matrix: ") << m_vscreen_2_screen << " s2vs matrix: " << m_screen_2_vscreen);
	}

	void Camera::calculateZValue(ScreenPoint& screen_coords) {
		int32_t dy = -(screen_coords.y - toScreenCoordinates(m_location.getMapCoordinates()).y);
		screen_coords.z = static_cast<int32_t>(Mathd::Tan(m_tilt * (Mathd::pi() / 180.0)) * static_cast<double>(dy));
	}

	ExactModelCoordinate Camera::toMapCoordinates(ScreenPoint screen_coords, bool z_calculated) {
		if (!z_calculated) {
			calculateZValue(screen_coords);
		}
		return m_inverse_matrix  * intPt2doublePt(screen_coords);
	}

	ScreenPoint Camera::toScreenCoordinates(const ExactModelCoordinate& elevation_coords) {
		ScreenPoint pt = doublePt2intPt(m_matrix * elevation_coords);
		return pt;
	}

	DoublePoint3D Camera::toVirtualScreenCoordinates(const ExactModelCoordinate& elevation_coords) {
		DoublePoint3D pt = (m_vs_matrix * elevation_coords);
		return pt;
	}

	ScreenPoint Camera::virtualScreenToScreen(const DoublePoint3D& p) {
		return doublePt2intPt(m_vscreen_2_screen * p);
	}

	DoublePoint3D Camera::screenToVirtualScreen(const ScreenPoint& p) {
		return m_screen_2_vscreen * intPt2doublePt(p);
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

		double x1 = 0;
		double x2 = 0;
		double y1 = 0;
		double y2 = 0;

		for (uint32_t i = 0; i < vertices.size(); i++) {
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

	bool Camera::cacheNeedUpdate(Layer* layer) {
		LayerCache* cache = m_cache[layer];
		if(!cache) {
			return true;
		}

		if (m_iswarped || !m_updated || cache->needUpdate()) {
			return true;
		}

		return false;
	}

	void Camera::getMatchingInstances(ScreenPoint screen_coords, Layer& layer, std::list<Instance*>& instances, uint8_t alpha) {
		instances.clear();
		bool zoomed = !Mathd::Equal(m_zoom, 1.0);
		bool update = cacheNeedUpdate(&layer);
		bool special_alpha = alpha != 0;
		const RenderList& layer_instances = m_layer_to_instances[&layer];
		RenderList::const_iterator instance_it = layer_instances.end();
		while (instance_it != layer_instances.begin()) {
			--instance_it;
			Instance* i = (*instance_it)->instance;
			const RenderItem& vc = **instance_it;
			if ((vc.dimensions.contains(Point(screen_coords.x, screen_coords.y)))) {
				assert(vc.image.get());
				uint8_t r, g, b, a = 0;
				int32_t x = screen_coords.x - vc.dimensions.x;
				int32_t y = screen_coords.y - vc.dimensions.y;
				if (zoomed) {
					double fx = static_cast<double>(x);
					double fy = static_cast<double>(y);
					double fow = static_cast<double>(vc.image->getWidth());
					double foh = static_cast<double>(vc.image->getHeight());
					double fsw = static_cast<double>(vc.dimensions.w);
					double fsh = static_cast<double>(vc.dimensions.h);
					x = static_cast<int32_t>(round(fx / fsw * fow));
					y = static_cast<int32_t>(round(fy / fsh * foh));
				}
				vc.image->getPixelRGBA(x, y, &r, &g, &b, &a);
				// instance is hit with mouse if not totally transparent
				if (a == 0 || (special_alpha && a < alpha)) {
					continue;
				}
				if (update) {
					// convert ScreenCoordinates to MapCoordinates
					ScreenPoint sp(screen_coords.x-vc.image->getXShift(), screen_coords.y-vc.image->getYShift());
					const ExactModelCoordinate emc = toMapCoordinates(sp, false);
					Location loc(&layer);
					loc.setMapCoordinates(emc);
					// convert image size to LayerCoordinates
					Point p = getCellImageDimensions(&layer);
					int32_t x2 = static_cast<int32_t>(ceil(vc.image->getWidth() / static_cast<float>(p.x)));
					int32_t y2 = static_cast<int32_t>(ceil(vc.image->getHeight() / static_cast<float>(p.y)));
					// construct a Rect with LayerCoordinates to fetch instance
					Rect rec;
					rec.x = loc.getLayerCoordinates().x - x2;
					rec.y = loc.getLayerCoordinates().y - y2;
					rec.w = x2*2;
					rec.h = y2*2;
					// use InstanceTree for fast picking
					std::list<Instance*> inslist = layer.getInstancesIn(rec);
					for (std::list<Instance*>::iterator it = inslist.begin(); it != inslist.end(); ++it) {
						if (i == *it) {
							instances.push_back(i);
							break;
						}
					}
				} else {
					instances.push_back(i);
				}
			}
		}
	}

	void Camera::getMatchingInstances(Rect screen_rect, Layer& layer, std::list<Instance*>& instances, bool accurate, uint8_t alpha) {
		instances.clear();
		bool zoomed = !Mathd::Equal(m_zoom, 1.0);
		bool update = cacheNeedUpdate(&layer);
		bool special_alpha = alpha != 0;
		std::list<Instance*> tree_instances;
		if (update || !accurate) {
			// convert screen_rect to tree_rect and use it to collect instances
			std::vector<ScreenPoint> points;
			ScreenPoint sp(screen_rect.x, screen_rect.y);
			points.push_back(sp);
			sp.y = screen_rect.y+screen_rect.h;
			points.push_back(sp);
			sp.x = screen_rect.x+screen_rect.w;
			points.push_back(sp);
			sp.y = screen_rect.y;
			points.push_back(sp);

			Location loc(&layer);
			ModelCoordinate max(-1000000,-1000000);
			ModelCoordinate min(1000000,1000000);
			for (std::vector<ScreenPoint>::iterator it = points.begin(); it != points.end(); ++it) {
				loc.setMapCoordinates(toMapCoordinates(*it, false));
				ModelCoordinate mc = loc.getLayerCoordinates();
				max.x = std::max(max.x, mc.x);
				max.y = std::max(max.y, mc.y);
				min.x = std::min(min.x, mc.x);
				min.y = std::min(min.y, mc.y);
			}

			Rect tree_rect(min.x, min.y, max.x-min.x, max.y-min.y);
			tree_instances = layer.getInstancesIn(tree_rect);

			if (!accurate) {
				for (std::list<Instance*>::iterator insit = tree_instances.begin(); insit != tree_instances.end(); ++insit) {
					InstanceVisual* visual = (*insit)->getVisual<InstanceVisual>();
					if (!visual) {
						continue;
					}
					// check if instance is visible
					if (visual->isVisible() && (visual->getTransparency() < 255 ||
						(special_alpha && (255-visual->getTransparency()) >= alpha))) {
						instances.push_back(*insit);
					}
				}
				return;
			}
		}

		const RenderList& layer_instances = m_layer_to_instances[&layer];
		RenderList::const_iterator instance_it = layer_instances.end();
		while (instance_it != layer_instances.begin()) {
			--instance_it;
			Instance* i = (*instance_it)->instance;;
			const RenderItem& vc = **instance_it;
			if ((vc.dimensions.intersects(screen_rect))) {
				assert(vc.image.get());
				uint8_t r, g, b, a = 0;
				for(int32_t xx = screen_rect.x; xx < screen_rect.x + screen_rect.w; xx++) {
					for(int32_t yy = screen_rect.y; yy < screen_rect.y + screen_rect.h; yy++) {
						if ((vc.dimensions.contains(Point(xx, yy)))) {
							int32_t x = xx - vc.dimensions.x;
							int32_t y = yy - vc.dimensions.y;
							if (zoomed) {
								double fx = static_cast<double>(x);
								double fy = static_cast<double>(y);
								double fow = static_cast<double>(vc.image->getWidth());
								double foh = static_cast<double>(vc.image->getHeight());
								double fsw = static_cast<double>(vc.dimensions.w);
								double fsh = static_cast<double>(vc.dimensions.h);
								x = static_cast<int32_t>(round(fx / fsw * fow));
								y = static_cast<int32_t>(round(fy / fsh * foh));
							}
							vc.image->getPixelRGBA(x, y, &r, &g, &b, &a);
							// instance is hit with mouse if not totally transparent
							if (a == 0 || (special_alpha && a < alpha)) {
								continue;
							}

							if (update) {
								for (std::list<Instance*>::iterator it = tree_instances.begin(); it != tree_instances.end(); ++it) {
									// if the instance has been found in tree then it should be a valid instance
									if (i == *it) {
										instances.push_back(i);
										tree_instances.erase(it);
										goto found_non_transparent_pixel;
									}
								}
							} else {
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
		Layer* layer = loc.getLayer();
		if(!layer) {
			return;
		}

		if (cacheNeedUpdate(layer)) {
			std::vector<Instance*> insvec = layer->getInstancesAt(loc, use_exactcoordinates);
			for (std::vector<Instance*>::iterator it = insvec.begin(); it != insvec.end(); ++it) {
				InstanceVisual* visual = (*it)->getVisual<InstanceVisual>();
				// check if instance is visible
				if (visual->isVisible() && visual->getTransparency() < 255) {
					instances.push_back(*it);
				}
			}
		} else {
			const RenderList& layer_instances = m_layer_to_instances[layer];
			RenderList::const_iterator instance_it = layer_instances.end();
			while (instance_it != layer_instances.begin()) {
				--instance_it;
				Instance* i = (*instance_it)->instance;
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
		m_updated = true;
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

	void Camera::resetRenderers() {
		std::map<std::string, RendererBase*>::iterator r_it = m_renderers.begin();
		for (; r_it != m_renderers.end(); ++r_it) {
			r_it->second->reset();
		}
	}

	void Camera::addLayer(Layer* layer) {
		m_cache[layer] = new LayerCache(this);
		m_cache[layer]->setLayer(layer);
		m_layer_to_instances[layer] = RenderList();
	}

	void Camera::removeLayer(Layer* layer) {
		delete m_cache[layer];
		m_cache.erase(layer);
		m_layer_to_instances.erase(layer);
	}

	void Camera::setLightingColor(float red, float green, float blue) {
		m_lighting = true;
		m_light_colors.clear();
		m_light_colors.push_back(red);
		m_light_colors.push_back(green);
		m_light_colors.push_back(blue);
	}

	std::vector<float> Camera::getLightingColor() {
		if(m_light_colors.empty()) {
			for(int32_t colors = 0; colors != 3; ++colors) {
				m_light_colors.push_back(1.0f);
			}
		}
		return m_light_colors;
	}

	void Camera::resetLightingColor() {
		m_lighting = false;
		m_renderbackend->resetLighting();
	}

	void Camera::setOverlayColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
		m_col_overlay = true;
		m_overlay_color.r = red;
		m_overlay_color.g = green;
		m_overlay_color.b = blue;
		m_overlay_color.unused = alpha;
	}

	std::vector<uint8_t> Camera::getOverlayColor() {
		std::vector<uint8_t> colors;
		if (m_col_overlay) {
			colors.push_back(m_overlay_color.r);
			colors.push_back(m_overlay_color.g);
			colors.push_back(m_overlay_color.b);
			colors.push_back(m_overlay_color.unused);
		} else {
			for(uint8_t cc = 0; cc != 4; ++cc) {
				colors.push_back(255);
			}
		}
		return colors;
	}

	void Camera::resetOverlayColor() {
		m_col_overlay = false;
	}

	void Camera::setOverlayImage(int32_t id, bool fill) {
		m_img_overlay = true;
		m_img_id = id;
		m_img_fill = fill;
	}

	int32_t Camera::getOverlayImage() {
		int32_t id = -1;
		if (m_img_overlay) {
			id = m_img_id;
		}
		return id;
	}

	void Camera::resetOverlayImage() {
		m_img_overlay = false;
		m_img_id = -1;
	}

	void Camera::setOverlayAnimation(AnimationPtr anim, bool fill) {
		m_ani_overlay = true;
		m_ani_ptr = anim;
		m_ani_fill = fill;
		m_start_time = 0;
	}

	AnimationPtr Camera::getOverlayAnimation() {
		return m_ani_ptr;
	}

	void Camera::resetOverlayAnimation() {
		m_ani_overlay = false;
		m_ani_ptr.reset();
	}

	void Camera::renderOverlay() {
		if (!m_col_overlay && !m_img_overlay && !m_ani_overlay) {
			return;
		}
		uint16_t width = m_viewport.w;
		uint16_t height = m_viewport.h;
		Point pm = Point(m_viewport.x + width/2, m_viewport.y + height/2);
		Rect r;

		// color overlay
		if (m_col_overlay) {
			Point p = Point(m_viewport.x, m_viewport.y);
			m_renderbackend->fillRectangle(p, width, height, m_overlay_color.r, m_overlay_color.g, m_overlay_color.b, m_overlay_color.unused);
		}
		// image overlay
		if (m_img_overlay) {
			ImagePtr resptr = ImageManager::instance()->get(m_img_id);
			Image* img = resptr.get();
			if (img) {
				if (m_img_fill) {
					r.w = width;
					r.h = height;
				} else {
					r.w = img->getWidth();
					r.h = img->getHeight();
				}
				r.x = pm.x-r.w/2;
				r.y = pm.y-r.h/2;
				img->render(r);
			}
		}
		// animation overlay
		if (m_ani_overlay) {
			assert(m_ani_ptr != 0);

			if (m_start_time == 0) {
				m_start_time = TimeManager::instance()->getTime();
			}
			uint32_t animtime = scaleTime(1.0, TimeManager::instance()->getTime() - m_start_time) % m_ani_ptr->getDuration();
			ImagePtr img = m_ani_ptr->getFrameByTimestamp(animtime);
			if (img) {
				if (m_ani_fill) {
					r.w = width;
					r.h = height;
				} else {
					r.w = img->getWidth();
					r.h = img->getHeight();
				}
				r.x = pm.x-r.w/2;
				r.y = pm.y-r.h/2;
				img->render(r);
			}
		}
	}

	void Camera::updateRenderLists() {
		Map* map = m_location.getMap();
		if (!map) {
			FL_ERR(_log, "No map for camera found");
			return;
		}

		Transform transform = NormalTransform;
		if (m_iswarped) {
			transform = WarpedTransform;
		}
		const std::list<Layer*>& layers = map->getLayers();
		std::list<Layer*>::const_iterator layer_it = layers.begin();
		for (;layer_it != layers.end(); ++layer_it) {
			LayerCache* cache = m_cache[*layer_it];
			if(!cache) {
				addLayer(*layer_it);
				cache = m_cache[*layer_it];
				FL_ERR(_log, LMsg("Layer Cache miss! (This shouldn't happen!)") << (*layer_it)->getId());
			}
			RenderList& instances_to_render = m_layer_to_instances[*layer_it];
			if (m_iswarped || !m_updated || cache->needUpdate()) {
				cache->update(transform, instances_to_render);
			}
		}
		resetUpdates();
	}

	void Camera::render() {
		static bool renderbackendOpenGLe = (m_renderbackend->getName() == "OpenGLe");
		updateRenderLists();
		Map* map = m_location.getMap();
		if (!map) {
			return;
		}

		uint32_t lm = m_renderbackend->getLightingModel();
		if (lm != 0) {
			m_renderbackend->resetStencilBuffer(0);
			if (m_lighting) {
				m_renderbackend->setLighting(m_light_colors[0], m_light_colors[1], m_light_colors[2]);
			}
		}

		m_renderbackend->pushClipArea(getViewPort());

		const std::list<Layer*>& layers = map->getLayers();
		std::list<Layer*>::const_iterator layer_it = layers.begin();
		for ( ; layer_it != layers.end(); ++layer_it) {
			RenderList& instances_to_render = m_layer_to_instances[*layer_it];
			std::list<RendererBase*>::iterator r_it = m_pipeline.begin();
			for (; r_it != m_pipeline.end(); ++r_it) {
				if ((*r_it)->isActivedLayer(*layer_it)) {
					(*r_it)->render(this, *layer_it, instances_to_render);
				}
			}
			if (renderbackendOpenGLe) {
				m_renderbackend->renderVertexArrays();
			}
		}

		renderOverlay();
		m_renderbackend->renderVertexArrays();
		if (m_lighting && lm != 0) {
			m_renderbackend->resetLighting();
		}
		m_renderbackend->popClipArea();
	}
}
