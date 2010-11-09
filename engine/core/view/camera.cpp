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
#include "util/math/angles.h"
#include "util/time/timemanager.h"
#include "video/renderbackend.h"
#include "video/image.h"
#include "video/imagepool.h"
#include "video/animation.h"
#include "video/animationpool.h"

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
			m_layer_to_instances(),
			m_lighting(false),
			m_light_colors() {

		m_viewport = viewport;
		m_map_observer = new MapObserver(this);
		m_map = 0;
		Location location;
		location.setLayer(layer);
		setLocation(location);
		if(m_renderbackend->getName() == "SDL") {
			m_backendSDL = true;
		} else {
			m_backendSDL = false;
		}
	}

	Camera::~Camera() {
		// Trigger removal of LayerCaches and MapObserver
		updateMap(NULL);

		std::map<std::string, RendererBase*>::iterator r_it = m_renderers.begin();
		for(; r_it != m_renderers.end(); ++r_it) {
			delete r_it->second;
		}
		m_renderers.clear();
		delete m_map_observer;
	}

	void Camera::setTilt(double tilt) {
		if(m_tilt != tilt) {
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
		if(m_rotation != rotation) {
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
		if(m_zoom!=zoom) {
			m_zoom = zoom;
			if (m_zoom < 0.001) {
				m_zoom = 0.001;
			}
			updateMatrices();
		}
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

		// WARNING
		// It is important that m_location is already set,
		// as the updates which are triggered here
		// need to calculate screen-coordinates
		// which depend on m_location.
		updateMap(location.getMap());

		m_cur_origo = toScreenCoordinates(ExactModelCoordinate(0,0,0));
	}

	void Camera::updateMap(Map* map)
	{
		if(m_map == map)
		{
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
			for(std::list<Layer*>::const_iterator i = layers.begin(); i !=layers.end(); ++i)
				addLayer(*i);
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


		m_vs_matrix.applyTranslate(0,0,0);
		m_vs_matrix.applyRotate(-m_rotation, 0.0, 0.0, 1.0);
		m_vs_matrix.applyRotate(-m_tilt, 1.0, 0.0, 0.0);
		m_vs_inverse_matrix = m_vs_matrix.inverse();

		// calculate the screen<->virtual screen transformation
		// this explicitly ignores the z-value.
		m_vscreen_2_screen = m_matrix;
		// NOTE: mult4by4 is an in-place modification.
		m_vscreen_2_screen.mult4by4(m_vs_inverse_matrix);
		// set the z transformation to unity
		const int N=4;
		for(int i=0; i!=N; ++i) {
			  m_vscreen_2_screen[2*N + i] = 0;
			  m_vscreen_2_screen[i*N + 2] = 0;
		}
		m_vscreen_2_screen[2*N + 2] = 1;
		m_screen_2_vscreen = m_vscreen_2_screen.inverse();

		// FL_WARN(_log, LMsg("matrix: ") << m_matrix << " 1: " << m_matrix.inverse().mult4by4(m_matrix));
// 		FL_WARN(_log, LMsg("vs2s matrix: ") << m_vscreen_2_screen << " s2vs matrix: " << m_screen_2_vscreen);
	}

	void Camera::calculateZValue(ScreenPoint& screen_coords) {
		int dy = -(screen_coords.y - toScreenCoordinates(m_location.getMapCoordinates()).y);
		screen_coords.z = static_cast<int>(Mathd::Tan(m_tilt * (Mathd::pi() / 180.0)) * static_cast<double>(dy));
	}

	ExactModelCoordinate Camera::toMapCoordinates(ScreenPoint screen_coords, bool z_calculated) {
		if (!z_calculated) {
			calculateZValue(screen_coords);
		}
		return m_inverse_matrix  * intPt2doublePt(screen_coords);
	}

	ScreenPoint Camera::toScreenCoordinates(ExactModelCoordinate elevation_coords) {
		ExactModelCoordinate p = elevation_coords;
		ScreenPoint pt = doublePt2intPt( m_matrix* p  );
		return pt;
	}

	DoublePoint3D  Camera::toVirtualScreenCoordinates(ExactModelCoordinate elevation_coords) {
		ExactModelCoordinate p = elevation_coords;
		DoublePoint3D  pt = (m_vs_matrix * p);
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

	bool Camera::testRenderedViewPort() {
		Map* map = m_location.getMap();
		Rect cv = m_viewport;
		int cv2x = cv.x+cv.w;
		int cv2y = cv.y+cv.h;
		bool trec1 = false, trec2 = false, trec3 = false, trec4 = false;
		Rect rec1 = Rect(cv.x, cv.y, 1, 1);
		Rect rec2 = Rect(cv.x, cv2y, 1, 1);
		Rect rec3 = Rect(cv2x, cv.y, 1, 1);
		Rect rec4 = Rect(cv2x, cv2y, 1, 1);

		const std::list<Layer*>& layers = map->getLayers();
		std::list<Layer*>::const_iterator layer_it = layers.begin();
		m_layer_to_instances.clear();
		const RenderList& layer_instances = m_layer_to_instances[*layer_it];
		RenderList::const_iterator instance_it = layer_instances.begin();
		for(; instance_it != layer_instances.end(); ++instance_it) {
			const RenderItem& vc = **instance_it;
			if(vc.dimensions.intersects(rec1) && !trec1) {
				trec1 = true;
			}
			if(vc.dimensions.intersects(rec2) && !trec2) {
				trec2 = true;
			}
			if(trec1 && trec2) {
				break;
			}
		}
		if(trec1 && trec2) {
			RenderList::const_reverse_iterator instance_itr = layer_instances.rbegin();
			for(; instance_itr != layer_instances.rend(); ++instance_itr) {
				const RenderItem& vc = **instance_itr;
				if(vc.dimensions.intersects(rec3) && !trec3) {
					trec3 = true;
				}
				if(vc.dimensions.intersects(rec4) && !trec4) {
					trec4 = true;
				}
				if(trec3 && trec4) {
					break;
				}
			}
		}

		if(trec1 && trec2 && trec3 && trec4) {
			return false;
		}
		return true;
	}

	void Camera::getMatchingInstances(ScreenPoint screen_coords, Layer& layer, std::list<Instance*>& instances) {
		instances.clear();
		const RenderList& layer_instances = m_layer_to_instances[&layer];
		RenderList::const_iterator instance_it = layer_instances.end();
		while (instance_it != layer_instances.begin()) {
			--instance_it;
			Instance* i = (*instance_it)->instance;
			const RenderItem& vc = **instance_it;
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
		const RenderList& layer_instances = m_layer_to_instances[&layer];
		RenderList::const_iterator instance_it = layer_instances.end();
		while (instance_it != layer_instances.begin()) {
			--instance_it;
			Instance* i = (*instance_it)->instance;;
			const RenderItem& vc = **instance_it;
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
		const RenderList& layer_instances = m_layer_to_instances[loc.getLayer()];
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

	void Camera::resetRenderers() {
		std::map<std::string, RendererBase*>::iterator r_it = m_renderers.begin();
		for (; r_it != m_renderers.end(); ++r_it) {
			Map* map = m_location.getMap();
			r_it->second->reset();
		}
	}

	void Camera::addLayer(Layer* layer) {
		m_cache[layer] = new LayerCache(this, m_ipool, m_apool);
		m_cache[layer]->setLayer(layer);
		m_layer_to_instances[layer] = RenderList();
	}

	void Camera::removeLayer(Layer* layer) {
		delete m_cache[layer];
		m_cache.erase(layer);
		m_layer_to_instances.erase(layer);
	}

	void Camera::setLightingColor(float red, float green, float blue, float alpha) {
		m_lighting = true;
		m_light_colors.clear();
		m_light_colors.push_back(red);
		m_light_colors.push_back(green);
		m_light_colors.push_back(blue);
		m_light_colors.push_back(alpha);
	}

	std::vector<float> Camera::getLightingColor() {
		if(m_light_colors.empty()) {
			for(int colors = 0; colors != 4; ++colors) {
				m_light_colors.push_back(1.0f);
			}
		}
		return m_light_colors;
	}

	void Camera::resetLightingColor() {
		m_lighting = false;
		m_renderbackend->resetLighting();
	}

	void Camera::render() {
		Transform transform = NormalTransform;
		if(m_iswarped)
			  transform = WarpedTransform;
		m_iswarped = false;

		Map* map = m_location.getMap();
		if (!map) {
			FL_ERR(_log, "No map for camera found");
			return;
		}
		//if ((!map->isChanged()) && (!m_iswarped) && (cammove == ScreenPoint(0,0,0))) {
		//	return;
		//}

		if (m_renderbackend->getLightingModel() != 0) {
			m_renderbackend->resetStencilBuffer(0);
			if (m_lighting) {
				m_renderbackend->setLighting(m_light_colors[0], m_light_colors[1], m_light_colors[2], m_light_colors[3]);
			}
		}

		if(m_backendSDL) {
			m_renderbackend->pushClipArea(getViewPort());
		} else {
			m_renderbackend->pushClipArea(getViewPort(), testRenderedViewPort());
		}

		// update each layer
// 		m_layer_to_instances.clear();

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
			cache->update(transform, instances_to_render);

			std::list<RendererBase*>::iterator r_it = m_pipeline.begin();
			for (; r_it != m_pipeline.end(); ++r_it) {
				if ((*r_it)->isActivedLayer(*layer_it)) {
					(*r_it)->render(this, *layer_it, instances_to_render);
				}
			}
		}

		if (m_lighting) {
			m_renderbackend->resetLighting();
		}

		m_renderbackend->popClipArea();
		resetUpdates();
		m_updated = true;
	}

}
