/***************************************************************************
 *   Copyright (C) 2005-2012 by the FIFE team                              *
 *   http://www.fifengine.net                                               *
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
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_CAMERA);

	const double OVERDRAW = 1.5;

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

	class InstanceDistanceSort {
	public:
		InstanceDistanceSort(double rotation) {
			m_xSign = (Mathd::Sin(rotation*Mathd::pi()/180) < 0) - (Mathd::Sin(rotation*Mathd::pi()/180) > 0);
			m_ySign = (Mathd::Cos(rotation*Mathd::pi()/180) > 0) - (Mathd::Cos(rotation*Mathd::pi()/180) < 0);
		}

		inline bool operator()(RenderItem* const & lhs, RenderItem* const & rhs) {
			const ExactModelCoordinate& lpos = lhs->instance->getLocationRef().getExactLayerCoordinatesRef();
			const ExactModelCoordinate& rpos = rhs->instance->getLocationRef().getExactLayerCoordinatesRef();
			if (ceil(m_xSign*lpos.x) + ceil(m_ySign*lpos.y) == ceil(m_xSign*rpos.x) + ceil(m_ySign*rpos.y)) {
				if (Mathd::Equal(lpos.z, rpos.z)) {
					InstanceVisual* liv = lhs->instance->getVisual<InstanceVisual>();
					InstanceVisual* riv = rhs->instance->getVisual<InstanceVisual>();
					return liv->getStackPosition() < riv->getStackPosition();
				}
				return lpos.z < rpos.z;
			}
			return ceil(m_xSign*lpos.x) + ceil(m_ySign*lpos.y) < ceil(m_xSign*rpos.x) + ceil(m_ySign*rpos.y);
		}
	private:
		int8_t m_xSign;
		int8_t m_ySign;
	};

	LayerCache::LayerCache(Camera* camera) {
		m_camera = camera;
		m_layer = 0;
		m_tree = 0;
		m_zMin = 0.0;
		m_zMax = 0.0;
		m_zoom = camera->getZoom();
		m_zoomed = !Mathd::Equal(m_zoom, 1.0);
		m_straightZoom = Mathd::Equal(fmod(m_zoom, 1.0), 0.0);
		
		if(RenderBackend::instance()->getName() == "OpenGLe") {
			m_needSorting = false;
		} else {
			m_needSorting = true;
		}
	}

	LayerCache::~LayerCache() {
		// removes all Entries
		for (std::vector<Entry*>::iterator it = m_entries.begin(); it != m_entries.end(); ++it) {
			delete *it;
		}
		// removes all RenderItems
		for (std::vector<RenderItem*>::iterator it = m_renderItems.begin(); it != m_renderItems.end(); ++it) {
			delete *it;
		}
		m_layer->removeChangeListener(m_layerObserver);
		delete m_layerObserver;
		delete m_tree;
	}

	void LayerCache::setLayer(Layer* layer) {
		if (m_layer != layer) {
			if (m_layer) {
				m_layer->removeChangeListener(m_layerObserver);
				delete m_layerObserver;
			}
			m_layer = layer;
			m_layerObserver = new CacheLayerChangeListener(this);
			layer->addChangeListener(m_layerObserver);
			reset();
		}
	}

	void LayerCache::reset() {
		// removes all Entries
		for (std::vector<Entry*>::iterator it = m_entries.begin(); it != m_entries.end(); ++it) {
			delete *it;
		}
		m_entries.clear();
		// removes all RenderItems
		for (std::vector<RenderItem*>::iterator it = m_renderItems.begin(); it != m_renderItems.end(); ++it) {
			delete *it;
		}
		m_renderItems.clear();
		m_instance_map.clear();
		m_entriesToUpdate.clear();
		m_freeEntries.clear();

		delete m_tree;
		m_tree = new CacheTree;
		const std::vector<Instance*>& instances = m_layer->getInstances();
		for(std::vector<Instance*>::const_iterator i = instances.begin();
			i != instances.end(); ++i) {
			addInstance(*i);
		}
	}

	void LayerCache::addInstance(Instance* instance) {
		assert(m_instance_map.find(instance) == m_instance_map.end());

		RenderItem* item;
		Entry* entry;
		if (m_freeEntries.empty()) {
			// creates new RenderItem
			item = new RenderItem();
			m_renderItems.push_back(item);
			m_instance_map[instance] = m_renderItems.size() - 1;
			// creates new Entry
			entry = new Entry();
			m_entries.push_back(entry);
			entry->instance_index = m_renderItems.size() - 1;
			entry->entry_index = m_entries.size() - 1;
		} else {
			// uses free/unused RenderItem and Entry
			int32_t index = m_freeEntries.front();
			m_freeEntries.pop_front();
			item = m_renderItems[index];
			// reset dimension needed because of the on screen check
			item->dimensions = Rect();
			m_instance_map[instance] = index;
			
			entry = m_entries[index];
			entry->instance_index = index;
			entry->entry_index = index;
		}

		item->instance = instance;
		item->transparency = 255;

		entry->node = 0;
		entry->force_update = true;
		entry->visible = true;
		entry->updateInfo = EntryFullUpdate;

		m_entriesToUpdate.insert(entry->entry_index);
	}

	void LayerCache::removeInstance(Instance* instance) {
		assert(m_instance_map.find(instance) != m_instance_map.end());

		Entry* entry = m_entries[m_instance_map[instance]];
		assert(entry->instance_index == m_instance_map[instance]);
		RenderItem* item = m_renderItems[entry->instance_index];
		// removes entry from updates
		std::set<int32_t>::iterator it = m_entriesToUpdate.find(entry->entry_index);
		if (it != m_entriesToUpdate.end()) {
			m_entriesToUpdate.erase(it);
		}
		// removes entrie from CacheTree
		if (entry->node) {
			entry->node->data().erase(entry->entry_index);
			entry->node = 0;
		}
		entry->instance_index = -1;
		entry->force_update = false;
		m_instance_map.erase(instance);

		// removes instance from RenderList
		RenderList& renderList = m_camera->getRenderListRef(m_layer);
		for (RenderList::iterator it = renderList.begin(); it != renderList.end(); ++it) {
			if ((*it)->instance == instance) {
				renderList.erase(it);
				break;
			}
		}
		item->instance = 0;
		
		// adds free entry
		m_freeEntries.push_back(entry->entry_index);
	}

	void LayerCache::updateInstance(Instance* instance) {
		Entry* entry = m_entries[m_instance_map[instance]];
		if (entry->instance_index == -1) {
			return;
		}
		bool inserted = (entry->updateInfo & EntryVisualUpdate) == EntryVisualUpdate;
		// convert necessary instance update flags to entry update flags
		const InstanceChangeInfo ici = instance->getChangeInfo();
		if ((ici & ICHANGE_LOC) == ICHANGE_LOC) {
			entry->updateInfo |= EntryPositionUpdate;
		}
		if ((ici & ICHANGE_ROTATION) == ICHANGE_ROTATION ||
			(ici & ICHANGE_ACTION) == ICHANGE_ACTION ||
			(ici & ICHANGE_TRANSPARENCY) == ICHANGE_TRANSPARENCY ||
			(ici & ICHANGE_VISIBLE) == ICHANGE_VISIBLE) {
			entry->updateInfo |= EntryVisualUpdate;
		}

		if (!inserted && entry->updateInfo != EntryNoneUpdate) {
			entry->force_update = true;
			m_entriesToUpdate.insert(entry->entry_index);
		}
	}

	class CacheTreeCollector {
			std::vector<int32_t>& m_indices;
			Rect m_viewport;
		public:
			CacheTreeCollector(std::vector<int32_t>& indices, const Rect& viewport)
			: m_indices(indices), m_viewport(viewport) {
			}
			bool visit(LayerCache::CacheTree::Node* node, int32_t d = -1);
	};

	bool CacheTreeCollector::visit(LayerCache::CacheTree::Node* node, int32_t d) {
		if(!m_viewport.intersects(Rect(node->x(), node->y(),node->size(),node->size()))) {
			return false;
		}
		m_indices.insert(m_indices.end(), node->data().begin(), node->data().end());
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

	void LayerCache::update(Camera::Transform transform, RenderList& renderlist) {
		if(!m_layer->areInstancesVisible()) {
			FL_DBG(_log, "Layer instances hidden");
			renderlist.clear();
			return;
		}
		// if transform is none then we have only to update the instances with an update info.
		if (transform == Camera::NoneTransform) {
			if (!m_entriesToUpdate.empty()) {
				std::set<int32_t> entryToRemove;
				updateEntries(entryToRemove, renderlist);
				//std::cout << "update entries: " << int32_t(m_entriesToUpdate.size()) << " remove entries: " << int32_t(entryToRemove.size()) <<"\n";
				if (!entryToRemove.empty()) {
					std::set<int32_t>::iterator entry_it = entryToRemove.begin();
					for (; entry_it != entryToRemove.end(); ++entry_it) {
						m_entriesToUpdate.erase(*entry_it);
					}
				}
			}
		} else {
			m_zoom = m_camera->getZoom();
			m_zoomed = !Mathd::Equal(m_zoom, 1.0);
			m_straightZoom = Mathd::Equal(fmod(m_zoom, 1.0), 0.0);
			// clear old renderlist
			renderlist.clear();
			// update all entries
			fullUpdate(transform);

			// create viewport coordinates to collect entries
			Rect viewport = m_camera->getViewPort();
			Rect screen_viewport = viewport;
			DoublePoint3D viewport_a = m_camera->screenToVirtualScreen(Point3D(viewport.x, viewport.y));
			DoublePoint3D viewport_b = m_camera->screenToVirtualScreen(Point3D(viewport.right(), viewport.bottom()));
			viewport.x = static_cast<int32_t>(std::min(viewport_a.x, viewport_b.x));
			viewport.y = static_cast<int32_t>(std::min(viewport_a.y, viewport_b.y));
			viewport.w = static_cast<int32_t>(std::max(viewport_a.x, viewport_b.x) - viewport.x);
			viewport.h = static_cast<int32_t>(std::max(viewport_a.y, viewport_b.y) - viewport.y);
			m_zMin = 0.0;
			m_zMax = 0.0;

			// FL_LOG(_log, LMsg("camera-update viewport") << viewport);
			std::vector<int32_t> index_list;
			collect(viewport, index_list);
			// fill renderlist
			for (uint32_t i = 0; i != index_list.size(); ++i) {
				Entry* entry = m_entries[index_list[i]];
				RenderItem* item = m_renderItems[entry->instance_index];
				if (!item->image || !entry->visible) {
					continue;
				}

				if(item->dimensions.intersects(screen_viewport)) {
					renderlist.push_back(item);
					if (!m_needSorting) {
						m_zMin = std::min(m_zMin, item->screenpoint.z);
						m_zMax = std::max(m_zMax, item->screenpoint.z);
					}
				}
			}

			if (m_needSorting) {
				sortRenderList(renderlist);
			} else {
				m_zMin -= 0.5;
				m_zMax += 0.5;
				sortRenderList(renderlist);
			}
		}
	}
	
	void LayerCache::fullUpdate(Camera::Transform transform) {
		bool rotationChange = (transform & Camera::RotationTransform) == Camera::RotationTransform;
		for (uint32_t i = 0; i != m_entries.size(); ++i) {
			Entry* entry = m_entries[i];
			if (entry->instance_index != -1) {
				if (rotationChange || entry->force_update) {
					updateVisual(entry);
				}
				updatePosition(entry);
			}
		}
	}

	void LayerCache::updateEntries(std::set<int32_t>& removes, RenderList& renderlist) {
		RenderList needSorting;
		Rect viewport = m_camera->getViewPort();
		std::set<int32_t>::const_iterator entry_it = m_entriesToUpdate.begin();
		for (; entry_it != m_entriesToUpdate.end(); ++entry_it) {
			Entry* entry = m_entries[*entry_it];
			entry->force_update = false;
			if (entry->instance_index == -1) {
				entry->updateInfo = EntryNoneUpdate;
				removes.insert(*entry_it);
				continue;
			}
			RenderItem* item = m_renderItems[entry->instance_index];
			bool positionUpdate = (entry->updateInfo & EntryPositionUpdate) == EntryPositionUpdate;

			if ((entry->updateInfo & EntryVisualUpdate) == EntryVisualUpdate) {
				positionUpdate |= updateVisual(entry);
			}

			if (positionUpdate) {
				bool onScreenA = item->dimensions.intersects(viewport);
				updatePosition(entry);
				bool onScreenB = item->dimensions.intersects(viewport);
				if (onScreenA != onScreenB) {
					if (!onScreenA && entry->visible && item->image) {
						// add to renderlist and sort
						renderlist.push_back(item);
						needSorting.push_back(item);
					} else {
						// remove from renderlist
						for (RenderList::iterator it = renderlist.begin(); it != renderlist.end(); ++it) {
							if ((*it)->instance == item->instance) {
								renderlist.erase(it);
								break;
							}
						}
					}
				} else if (onScreenA && onScreenB) {
					// sort
					needSorting.push_back(item);
				}
			}
			if (!entry->force_update || !entry->visible) {
				entry->updateInfo = EntryNoneUpdate;
				removes.insert(*entry_it);
			} else {
				entry->updateInfo = EntryVisualUpdate;
			}
		}

		if (!needSorting.empty()) {
			if (m_needSorting) {
				sortRenderList(renderlist);
			} else {
				sortRenderList(needSorting);
			}
		}
	}

	bool LayerCache::updateVisual(Entry* entry) {
		RenderItem* item = m_renderItems[entry->instance_index];
		Instance* instance = item->instance;
		InstanceVisual* visual = instance->getVisual<InstanceVisual>();
		item->facing_angle = instance->getRotation();
		int32_t angle = static_cast<int32_t>(m_camera->getRotation()) + item->facing_angle;
		Action* action = instance->getCurrentAction();
		ImagePtr image;

		if (visual) {
			uint8_t layer_trans = m_layer->getLayerTransparency();
			uint8_t instance_trans = visual->getTransparency();
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
			item->transparency = 255 - instance_trans;
			// only visible if visual is visible and item is not totally transparent
			entry->visible = (visual->isVisible() && item->transparency != 0);
		}

		if (!action) {
			// Try static images then default action.
			int32_t image_id = item->getStaticImageIndexByAngle(angle, instance);
			if (image_id == -1) {
				if (!instance->getObject()->isStatic()) {
					action = instance->getObject()->getDefaultAction();
				}
			} else {
				image = ImageManager::instance()->get(image_id);
			}
		}
		entry->force_update = (action != 0);

		if (action) {
			AnimationPtr animation = action->getVisual<ActionVisual>()->getAnimationByAngle(angle);
			unsigned animation_time = instance->getActionRuntime() % animation->getDuration();
			image = animation->getFrameByTimestamp(animation_time);

			int32_t action_frame = animation->getActionFrame();
			if (action_frame != -1) {
				if (item->image != image) {
					int32_t new_frame = animation->getFrameIndex(animation_time);
					if (action_frame == new_frame) {
						instance->callOnActionFrame(action, action_frame);
					// if action frame was skipped
					} else if (new_frame > action_frame && item->current_frame < action_frame) {
						instance->callOnActionFrame(action, action_frame);
					}
					item->current_frame = new_frame;
				}
			}
		}

		bool newPosition = false;
		if (image != item->image) {
			if (image) {
				if (!item->image) {
					newPosition = true;
				} else {
					if (image->getWidth() != item->image->getWidth() ||
						image->getHeight() != item->image->getHeight()) {
						newPosition = true;
					}
				}
			}
			item->image = image;
		}
		return newPosition;
	}

	void LayerCache::updatePosition(Entry* entry) {
		RenderItem* item = m_renderItems[entry->instance_index];
		Instance* instance = item->instance;
		ExactModelCoordinate map_coords = instance->getLocationRef().getMapCoordinates();
		DoublePoint3D screen_position = m_camera->toVirtualScreenCoordinates(map_coords);
		ImagePtr image = item->image;

		if (image) {
			int32_t w = image->getWidth();
			int32_t h = image->getHeight();
			screen_position.x = (screen_position.x - w / 2.0) + image->getXShift();
			screen_position.y = (screen_position.y - h / 2.0) + image->getYShift();
			item->bbox.w = w;
			item->bbox.h = h;
		} else {
			item->bbox.w = 0;
			item->bbox.h = 0;
		}
		item->screenpoint = screen_position;
		item->bbox.x = static_cast<int32_t>(screen_position.x);
		item->bbox.y = static_cast<int32_t>(screen_position.y);

		// seems wrong but these rounds fix the "wobbling" and gaps between tiles
		// in case the zoom is 1.0
		if (m_straightZoom) {
			item->screenpoint.x = round(item->screenpoint.x);
			item->screenpoint.y = round(item->screenpoint.y);
		}
		Point3D screen_point = m_camera->virtualScreenToScreen(item->screenpoint);
		// NOTE:
		// One would expect this to be necessary here,
		// however it works the same without, sofar
		// m_camera->calculateZValue(screen_point);
		// item->screenpoint.z = -screen_point.z;
		item->dimensions.x = screen_point.x;
		item->dimensions.y = screen_point.y;

		if (m_zoomed) {
			// NOTE: Due to image alignment, there is additional additions on image dimensions
			//       There's probabaly some better solution for this, but works "good enough" for now.
			//       In case additions are removed, gaps appear between tiles.
			//       This is only needed if the zoom is a non-integer value.
			if (!m_straightZoom) {
				item->dimensions.w = round(double(item->bbox.w) * m_zoom + OVERDRAW);
				item->dimensions.h = round(double(item->bbox.h) * m_zoom + OVERDRAW);
			} else {
				item->dimensions.w = round(double(item->bbox.w) * m_zoom);
				item->dimensions.h = round(double(item->bbox.h) * m_zoom);
			}
		} else {
			item->dimensions.w = item->bbox.w;
			item->dimensions.h = item->bbox.h;
		}

		CacheTree::Node* node = m_tree->find_container(item->bbox);
		if (node) {
			if (node != entry->node) {
				if (entry->node) {
					entry->node->data().erase(entry->entry_index);
				}
				entry->node = node;
				node->data().insert(entry->entry_index);
			}
		}
	}

	void LayerCache::sortRenderList(RenderList& renderlist) {
		if (renderlist.empty()) {
			return;
		}
		// We want to put every z value in [-10,10] range.
		// To do it, we simply solve
		// { y1 = a*x1 + b
		// { y2 = a*x2 + b
		// where [y1,y2]' = [-10,10]' is required z range,
		// and [x1,x2]' is expected min,max z coords.
		if (!m_needSorting) {
			double det = m_zMin - m_zMax;
			if (fabs(det) > FLT_EPSILON) {
				double det_a = -10.0 - 10.0;
				double det_b = 10.0 * m_zMin - (-10.0) * m_zMax;
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
		} else {
			InstanceDistanceSort ids(m_camera->getRotation());
			std::stable_sort(renderlist.begin(), renderlist.end(), ids);
		}
	}

	ImagePtr LayerCache::getCacheImage() {
		return m_cacheImage;
	}

	void LayerCache::setCacheImage(ImagePtr image) {
		m_cacheImage = image;
	}
}
