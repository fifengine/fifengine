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
#include <cfloat>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/action.h"
#include "model/structures/layer.h"
#include "model/structures/instance.h"
#include "model/structures/location.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"
#include "util/math/angles.h"
#include "video/renderbackend.h"
#include "video/image.h"
#include "video/animation.h"
#include "video/imagemanager.h"

#include "camera.h"
#include "layercache.h"
#include "visual.h"


namespace FIFE {
	static Logger _log(LM_CAMERA);

	class CacheLayerChangeListener : public LayerChangeListener {
	public:
		CacheLayerChangeListener(LayerCache* cache)	{
			m_cache = cache;
		}
		virtual ~CacheLayerChangeListener() {};

		virtual void onLayerChanged(Layer* layer, std::vector<Instance*>& instances) {
			for(std::vector<Instance*>::iterator i = instances.begin();
				i != instances.end(); ++i) {
				m_cache->updateInstance(*i);
			}
		}

		virtual void onInstanceCreate(Layer* layer, Instance* instance)	{
			m_cache->addInstance(instance);
		}

		virtual void onInstanceDelete(Layer* layer, Instance* instance)	{
			m_cache->removeInstance(instance);
		}
	private:
		LayerCache* m_cache;
	};

	LayerCache::LayerCache(Camera* camera) {
		m_camera = camera;
		m_layer = 0;
		m_tree = 0;
		m_needupdate = false;
		m_need_sorting = true;

		if(RenderBackend::instance()->getName() == "OpenGLe") {
			m_need_sorting = false;
		}
	}

	LayerCache::~LayerCache() {
		m_layer->removeChangeListener(m_layer_observer);
		delete m_layer_observer;
		delete m_tree;
	}

	void LayerCache::setLayer(Layer* layer) {
		m_layer = layer;
		m_layer_observer = new CacheLayerChangeListener(this);
		layer->addChangeListener(m_layer_observer);
		reset();
	}

	void LayerCache::reset() {
		m_instances.clear();
		delete m_tree;
		m_tree = new CacheTree;
		const std::vector<Instance*>& instances = m_layer->getInstances();
		for(std::vector<Instance*>::const_iterator i = instances.begin();
			i != instances.end(); ++i) {
			addInstance(*i);
		}
		m_needupdate = true;
	}

	void LayerCache::addInstance(Instance* instance) {
		if(m_instance_map.find(instance)!=m_instance_map.end()) {
			throw new Duplicate(instance->getId());
		}

		RenderItem item;
		Entry entry;
		item.instance = instance;
		m_instances.push_back(item);
		m_instance_map[instance] = m_instances.size() - 1;

		entry.node = 0;
		entry.instance_index = m_instances.size() - 1;
		entry.entry_index = m_entries.size();
		m_entries.push_back(entry);
		updateEntry(m_entries.back());
		m_needupdate = true;
	}

	void LayerCache::removeInstance(Instance* instance) {
		// FIXME
		// The way LayerCache stores it's data
		// it's pretty much impossible to cleanly remove
		// added instances.

		// This has to get fixed.
		if(m_instance_map.find(instance) == m_instance_map.end()) {
			throw new NotFound(instance->getId());
		}
		Entry& item = m_entries[m_instance_map[instance]];
		assert(item.instance_index == m_instance_map[instance]);

		if(item.node) {
			item.node->data().erase(item.entry_index);
		}
		item.node = 0;
		item.instance_index = -1;
		m_instance_map.erase(instance);
		m_needupdate = true;
	}

	void LayerCache::updateInstance(Instance* instance) {
		Entry& entry = m_entries[m_instance_map[instance]];
		updateEntry(entry);
		m_needupdate = true;
	}

	void LayerCache::updateEntry(LayerCache::Entry& item) {
		if(item.instance_index == -1) {
			return;
		}

		RenderItem& render_item = m_instances[item.instance_index];
		Instance* instance = render_item.instance;

		ExactModelCoordinate map_coords = instance->getLocationRef().getMapCoordinates();
		DoublePoint3D screen_position = m_camera->toVirtualScreenCoordinates(map_coords);

		render_item.facing_angle = getAngleBetween(instance->getLocationRef(), instance->getFacingLocation());
		int32_t angle = static_cast<int32_t>(m_camera->getRotation()) +
			render_item.facing_angle + instance->getRotation();

		ImagePtr image;
		Action* action = instance->getCurrentAction();
		int32_t w = 0;
		int32_t h = 0;

		if(!action) {
			// Try static images then default action.
			int32_t image_id = render_item.getStaticImageIndexByAngle(angle, instance);
			if(image_id == -1) {
				if (!instance->getObject()->isStatic()) {
					action = instance->getObject()->getDefaultAction();
				}
			} else {
				image = ImageManager::instance()->get(image_id);
			}
		}
		item.force_update = (action != 0);

		if(action) {
			AnimationPtr animation = action->getVisual<ActionVisual>()->getAnimationByAngle(
				render_item.facing_angle + static_cast<int32_t>(m_camera->getRotation()));
			unsigned animation_time = instance->getActionRuntime() % animation->getDuration();

			image = animation->getFrameByTimestamp(animation_time);

			int32_t action_frame = animation->getActionFrame();
			if (action_frame != -1) {
				if (render_item.image != image) {
					if (action_frame == animation->getFrameIndex(animation_time)) {
						instance->callOnActionFrame(action, action_frame);
					}
				}
			}

			int32_t facing_angle = render_item.facing_angle;
			if (facing_angle < 0){
				facing_angle += 360;
			}
			instance->setRotation(facing_angle);
			m_needupdate = true;
		}

		if (image) {
			w = image->getWidth();
			h = image->getHeight();

			screen_position.x -= w / 2;
			screen_position.x += image->getXShift();
			screen_position.y -= h / 2;
			screen_position.y += image->getYShift();
		}

		render_item.image = image;
		if (render_item.screenpoint == screen_position) {
			return;
		}
		render_item.screenpoint = screen_position;

		render_item.bbox.x = static_cast<int32_t>(screen_position.x);
		render_item.bbox.y = static_cast<int32_t>(screen_position.y);
		render_item.bbox.w = w;
		render_item.bbox.h = h;

		render_item.dimensions = render_item.bbox;

		CacheTree::Node* node = m_tree->find_container(render_item.bbox);
		if (node) {
			if(item.node) {
				item.node->data().erase(item.entry_index);
			}
			item.node = node;
			node->data().insert(item.entry_index);
		}
	}

	class CacheTreeCollector {
			std::vector<int32_t>& m_indices;
			Rect m_viewport;
		public:
			CacheTreeCollector(std::vector<int32_t>& indices, const Rect& _viewport)
			: m_indices(indices), m_viewport(_viewport) {
			}
			bool visit(LayerCache::CacheTree::Node* node, int32_t d = -1);
	};

	bool CacheTreeCollector::visit(LayerCache::CacheTree::Node* node, int32_t d) {
		if(!m_viewport.intersects(Rect(node->x(), node->y(),node->size(),node->size()))) {
			return false;
		}
		std::set<int32_t>& list = node->data();
		for(std::set<int32_t>::iterator i = list.begin(); i!=list.end();++i) {
			m_indices.push_back(*i);
		}
		return true;
	}

	void LayerCache::collect(const Rect& viewport, std::vector<int32_t>& index_list) {
		CacheTree::Node * node = m_tree->find_container(viewport);
		CacheTreeCollector collector(index_list, viewport);
		node->apply_visitor(collector);
		node = node->parent();
		while(node) {
			collector.visit(node);
			node = node->parent();
		}
	}

	void LayerCache::fullUpdate() {
		for(unsigned i=0; i!=m_entries.size(); ++i) {
			updateEntry(m_entries[i]);
		}
	}

	class InstanceDistanceSort {
	public:
		inline bool operator()(RenderItem* const & lhs, RenderItem* const & rhs) {
			if (lhs->screenpoint.z == rhs->screenpoint.z) {
				InstanceVisual* liv = lhs->instance->getVisual<InstanceVisual>();
				InstanceVisual* riv = rhs->instance->getVisual<InstanceVisual>();
				return liv->getStackPosition() < riv->getStackPosition();
			}
			return lhs->screenpoint.z < rhs->screenpoint.z;
		}
	};

	void LayerCache::update(Camera::Transform transform, RenderList& renderlist) {
		const double OVERDRAW = 2.5;
		renderlist.clear();
		m_needupdate = false;
		if(!m_layer->areInstancesVisible()) {
			FL_DBG(_log, "Layer instances hidden");
			return;
		}
		bool isWarped = transform == Camera::WarpedTransform;
		if( isWarped ) {
			fullUpdate();
		}

		Rect viewport = m_camera->getViewPort();
		Rect screen_viewport = viewport;
		double zoom = m_camera->getZoom();
		DoublePoint3D viewport_a = m_camera->screenToVirtualScreen(Point3D(viewport.x, viewport.y));
		DoublePoint3D viewport_b = m_camera->screenToVirtualScreen(Point3D(viewport.right(), viewport.bottom()));
		viewport.x = static_cast<int32_t>(std::min(viewport_a.x, viewport_b.x));
		viewport.y = static_cast<int32_t>(std::min(viewport_a.y, viewport_b.y));
		viewport.w = static_cast<int32_t>(std::max(viewport_a.x, viewport_b.x) - viewport.x);
		viewport.h = static_cast<int32_t>(std::max(viewport_a.y, viewport_b.y) - viewport.y);
		uint8_t layer_trans = m_layer->getLayerTransparency();

		double zmin = 0.0, zmax = 0.0;

		// FL_LOG(_log, LMsg("camera-update viewport") << viewport);
		std::vector<int32_t> index_list;
		collect(viewport, index_list);
		for(unsigned i=0; i!=index_list.size();++i) {
			Entry& entry = m_entries[index_list[i]];
			// NOTE
			// An update is forced if the item has an animation/action.
			// This update only happens if it is _already_ included in the viewport
			// Nevertheless: Moving instances - which might move into the viewport will be updated
			// By the layer change listener.
			if(entry.force_update || !isWarped) {
				updateEntry(entry);
			}

			RenderItem& item = m_instances[entry.instance_index];
			InstanceVisual* visual = item.instance->getVisual<InstanceVisual>();
			bool visible = (visual->isVisible() != 0);
			uint8_t instance_trans = visual->getTransparency();
			if(!item.image || !visible || (instance_trans == 255 && layer_trans == 0)
									   || (instance_trans == 0 && layer_trans == 255)) {
				continue;
			}

			if(layer_trans != 0) {
				if(instance_trans != 0) {
					uint8_t calc_trans = layer_trans - instance_trans;
					if(calc_trans >= 0) {
						instance_trans = calc_trans;
					} else {
						instance_trans = 0;
					}
				} else {
					instance_trans = layer_trans;
				}
			}

			Point3D screen_point = m_camera->virtualScreenToScreen(item.screenpoint);
			// NOTE:
			// One would expect this to be necessary here,
			// however it works the same without, sofar
			// m_camera->calculateZValue(screen_point);
			// item.screenpoint.z = -screen_point.z;

			item.dimensions.x = screen_point.x;
			item.dimensions.y = screen_point.y;
			item.dimensions.w = item.bbox.w;
			item.dimensions.h = item.bbox.h;

			item.transparency = 255 - instance_trans;

			if (zoom != 1.0) {
				// NOTE: Due to image alignment, there is additional additions on image dimensions
				//       There's probabaly some better solution for this, but works "good enough" for now.
				//       In case additions are removed, gaps appear between tiles.
				item.dimensions.w = unsigned(double(item.bbox.w) * zoom + OVERDRAW);
				item.dimensions.h = unsigned(double(item.bbox.h) * zoom + OVERDRAW);
			}

			if (!m_need_sorting) {
				zmin = std::min(zmin, item.screenpoint.z);
				zmax = std::max(zmax, item.screenpoint.z);
			}

			if(item.dimensions.intersects(screen_viewport)) {
				renderlist.push_back(&item);
			}
		}

		if (m_need_sorting) {
			InstanceDistanceSort ids;
			std::stable_sort(renderlist.begin(), renderlist.end(), ids);
		} else {
			zmin -= 0.5;
			zmax += 0.5;

			// We want to put every z value in [-10,10] range.
			// To do it, we simply solve 
			// { y1 = a*x1 + b
			// { y2 = a*x2 + b
			// where [y1,y2]' = [-10,10]' is required z range, 
			// and [x1,x2]' is expected min,max z coords.
			double det = zmin - zmax;
			if (fabs(det) > FLT_EPSILON) {
				double det_a = -10.0 - 10.0;
				double det_b = 10.0 * zmin - (-10.0) * zmax;
				double a = static_cast<float>(det_a / det);
				double b = static_cast<float>(det_b / det);
				float estimate = sqrtf(static_cast<float>(renderlist.size()));
				float stack_delta = fabs(-10.0f - 10.0f) / estimate * 0.1f;

				RenderList::iterator it = renderlist.begin();
				for ( ; it != renderlist.end(); ++it) {
					double& z = (*it)->screenpoint.z;
					z = a * z + b;
					InstanceVisual* vis = (*it)->instance->getVisual<InstanceVisual>();
					z += vis->getStackPosition() * stack_delta;					
				}
			}
		}
		//  FL_LOG(_log, LMsg("camera-update ") << " N=" <<renderlist.size() << "/" << m_instances.size() << "/" << index_list.size());
	}
}
