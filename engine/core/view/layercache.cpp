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

	/** Comparison functions for sorting
	*/
	// used screenpoint z for sorting, calculated from camera
	class InstanceDistanceSortCamera {
	public:
		inline bool operator()(RenderItem* const & lhs, RenderItem* const & rhs) {
			if (Mathd::Equal(lhs->screenpoint.z, rhs->screenpoint.z)) {
				InstanceVisual* liv = lhs->instance->getVisual<InstanceVisual>();
				InstanceVisual* riv = rhs->instance->getVisual<InstanceVisual>();
				return liv->getStackPosition() < riv->getStackPosition();
			}
			return lhs->screenpoint.z < rhs->screenpoint.z;
		}
	};
	// used instance location and camera rotation for sorting
	class InstanceDistanceSortLocation {
	public:
		InstanceDistanceSortLocation(double rotation) {
			if ((rotation >= 0) && (rotation <= 60)) { // 30 deg
				xtox = 0;
				xtoy = -1;
				ytox = 1;
				ytoy = 0.5;
			} else if ((rotation >= 60) && (rotation <= 120)) { // 90 deg
				xtox = -1;
				xtoy = -1;
				ytox = 0.5;
				ytoy = -0.5;
			} else if ((rotation >= 120) && (rotation <= 180)) { // 150 deg
				xtox = 0;
				xtoy = -1;
				ytox = -1;
				ytoy = -0.5;
			} else if ((rotation >= 180) && (rotation <= 240)) { // 210 deg
				xtox = 0;
				xtoy = 1;
				ytox = -1;
				ytoy = -0.5;
			} else if ((rotation >= 240) && (rotation <= 300)) { // 270 deg
				xtox = 1;
				xtoy = 1;
				ytox = -0.5;
				ytoy = 0.5;
			} else if ((rotation >= 300) && (rotation <= 360)) { // 330 deg
				xtox = 0;
				xtoy = 1;
				ytox = 1;
				ytoy = 0.5;
			}
		}

		inline bool operator()(RenderItem* const & lhs, RenderItem* const & rhs) {
			ExactModelCoordinate lpos = lhs->instance->getLocationRef().getExactLayerCoordinates();
			ExactModelCoordinate rpos = rhs->instance->getLocationRef().getExactLayerCoordinates();
			lpos.x += lpos.y / 2;
			rpos.x += rpos.y / 2;
			InstanceVisual* liv = lhs->instance->getVisual<InstanceVisual>();
			InstanceVisual* riv = rhs->instance->getVisual<InstanceVisual>();
			int32_t lvc = ceil(xtox*lpos.x + ytox*lpos.y) + ceil(xtoy*lpos.x + ytoy*lpos.y) + liv->getStackPosition();
			int32_t rvc = ceil(xtox*rpos.x + ytox*rpos.y) + ceil(xtoy*rpos.x + ytoy*rpos.y) + riv->getStackPosition();
			if (lvc == rvc) {
				if (Mathd::Equal(lpos.z, rpos.z)) {
					return liv->getStackPosition() < riv->getStackPosition();
				}
				return lpos.z < rpos.z;
			}
			return lvc < rvc;
		}
	private:
		double xtox;
		double xtoy;
		double ytox;
		double ytoy;
	};
	// used screenpoint z for sorting and as fallback first the instance location z and then the stack position
	class InstanceDistanceSortCameraAndLocation {
	public:
		inline bool operator()(RenderItem* const & lhs, RenderItem* const & rhs) {
			if (Mathd::Equal(lhs->screenpoint.z, rhs->screenpoint.z)) {
				const ExactModelCoordinate& lpos = lhs->instance->getLocationRef().getExactLayerCoordinatesRef();
				const ExactModelCoordinate& rpos = rhs->instance->getLocationRef().getExactLayerCoordinatesRef();
				if (Mathd::Equal(lpos.z, rpos.z)) {
					InstanceVisual* liv = lhs->instance->getVisual<InstanceVisual>();
					InstanceVisual* riv = rhs->instance->getVisual<InstanceVisual>();
					return liv->getStackPosition() < riv->getStackPosition();
				}
				return lpos.z < rpos.z;
			}
			return lhs->screenpoint.z < rhs->screenpoint.z;
		}
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
		
		if(RenderBackend::instance()->getName() == "OpenGL" && RenderBackend::instance()->isDepthBufferEnabled()) {
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
		m_cacheImage.reset();

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
			item = new RenderItem(instance);
			m_renderItems.push_back(item);
			m_instance_map[instance] = m_renderItems.size() - 1;
			// creates new Entry
			entry = new Entry();
			m_entries.push_back(entry);
			entry->instanceIndex = m_renderItems.size() - 1;
			entry->entryIndex = m_entries.size() - 1;
		} else {
			// uses free/unused RenderItem
			int32_t index = m_freeEntries.front();
			m_freeEntries.pop_front();
			item = m_renderItems[index];
			item->instance = instance;
			m_instance_map[instance] = index;
			// uses free/unused Entry
			entry = m_entries[index];
			entry->instanceIndex = index;
			entry->entryIndex = index;
		}

		entry->node = 0;
		entry->forceUpdate = true;
		entry->visible = true;
		entry->updateInfo = EntryFullUpdate;

		m_entriesToUpdate.insert(entry->entryIndex);
	}

	void LayerCache::removeInstance(Instance* instance) {
		assert(m_instance_map.find(instance) != m_instance_map.end());

		Entry* entry = m_entries[m_instance_map[instance]];
		assert(entry->instanceIndex == m_instance_map[instance]);
		RenderItem* item = m_renderItems[entry->instanceIndex];
		// removes entry from updates
		std::set<int32_t>::iterator it = m_entriesToUpdate.find(entry->entryIndex);
		if (it != m_entriesToUpdate.end()) {
			m_entriesToUpdate.erase(it);
		}
		// removes entry from CacheTree
		if (entry->node) {
			entry->node->data().erase(entry->entryIndex);
			entry->node = 0;
		}
		entry->instanceIndex = -1;
		entry->forceUpdate = false;
		m_instance_map.erase(instance);

		// removes instance from RenderList
		RenderList& renderList = m_camera->getRenderListRef(m_layer);
		for (RenderList::iterator it = renderList.begin(); it != renderList.end(); ++it) {
			if ((*it)->instance == instance) {
				renderList.erase(it);
				break;
			}
		}
		// resets RenderItem
		item->reset();
		// adds free entry
		m_freeEntries.push_back(entry->entryIndex);
	}

	void LayerCache::updateInstance(Instance* instance) {
		Entry* entry = m_entries[m_instance_map[instance]];
		if (entry->instanceIndex == -1) {
			return;
		}

		// convert necessary instance update flags to entry update flags
		const InstanceChangeInfo ici = instance->getChangeInfo();
		if ((ici & ICHANGE_LOC) == ICHANGE_LOC) {
			entry->updateInfo |= EntryPositionUpdate;
		}
		if ((ici & ICHANGE_ROTATION) == ICHANGE_ROTATION ||
			(ici & ICHANGE_ACTION) == ICHANGE_ACTION ||
			(ici & ICHANGE_TRANSPARENCY) == ICHANGE_TRANSPARENCY ||
			(ici & ICHANGE_VISIBLE) == ICHANGE_VISIBLE ||
			(ici & ICHANGE_VISUAL) == ICHANGE_VISUAL) {
			entry->updateInfo |= EntryVisualUpdate;
		}

		// if entry is not already inserted
		if (!entry->forceUpdate && entry->updateInfo != EntryNoneUpdate) {
			entry->forceUpdate = true;
			m_entriesToUpdate.insert(entry->entryIndex);
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
			if ((transform & Camera::RotationTransform) == Camera::RotationTransform ||
				(transform & Camera::TiltTransform) == Camera::TiltTransform ||
				(transform & Camera::ZTransform) == Camera::ZTransform) {
				fullUpdate(transform);
			} else {
				fullCoordinateUpdate(transform);
			}

			// create viewport coordinates to collect entries
			Rect viewport = m_camera->getViewPort();
			Rect screenViewport = viewport;
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
				RenderItem* item = m_renderItems[entry->instanceIndex];
				if (!item->image || !entry->visible) {
					continue;
				}

				if (item->dimensions.intersects(screenViewport)) {
					renderlist.push_back(item);
				}
			}

			if (m_needSorting) {
				sortRenderList(renderlist);
			} else {
				// calculates zmin and zmax of the current viewport
				Rect r = m_camera->getMapViewPort();
				std::vector<ExactModelCoordinate> coords;
				coords.push_back(ExactModelCoordinate(r.x, r.y));
				coords.push_back(ExactModelCoordinate(r.x, r.y+r.h));
				coords.push_back(ExactModelCoordinate(r.x+r.w, r.y));
				coords.push_back(ExactModelCoordinate(r.x+r.w, r.y+r.h));
				for (uint8_t i = 0; i < 4; ++i) {
					double z = m_camera->toVirtualScreenCoordinates(coords[i]).z;
					m_zMin = std::min(z, m_zMin);
					m_zMax = std::max(z, m_zMax);
				}

				sortRenderList(renderlist);
			}
		}
	}
	
	void LayerCache::fullUpdate(Camera::Transform transform) {
		bool rotationChange = (transform & Camera::RotationTransform) == Camera::RotationTransform;
		for (uint32_t i = 0; i != m_entries.size(); ++i) {
			Entry* entry = m_entries[i];
			if (entry->instanceIndex != -1) {
				if (rotationChange || entry->forceUpdate) {
					bool force = entry->forceUpdate;
					updateVisual(entry);
					if (force && !entry->forceUpdate) {
						// no action
						entry->updateInfo = EntryNoneUpdate;
						m_entriesToUpdate.erase(entry->entryIndex);
					} else if (!force && entry->forceUpdate) {
						// new action
						entry->updateInfo |= EntryVisualUpdate;
						m_entriesToUpdate.insert(entry->entryIndex);
					}
				}
				updatePosition(entry);
			}
		}
	}

	void LayerCache::fullCoordinateUpdate(Camera::Transform transform) {
		bool zoomChange = (transform & Camera::ZoomTransform) == Camera::ZoomTransform;
		for (uint32_t i = 0; i != m_entries.size(); ++i) {
			Entry* entry = m_entries[i];
			if (entry->instanceIndex != -1) {
				if (entry->forceUpdate) {
					updateVisual(entry);
					updatePosition(entry);
					if (!entry->forceUpdate) {
						// no action
						entry->updateInfo = EntryNoneUpdate;
						m_entriesToUpdate.erase(entry->entryIndex);
					}
					continue;
				}
				updateScreenCoordinate(m_renderItems[entry->instanceIndex], zoomChange);
			}
		}
	}

	void LayerCache::updateEntries(std::set<int32_t>& removes, RenderList& renderlist) {
		RenderList needSorting;
		Rect viewport = m_camera->getViewPort();
		std::set<int32_t>::const_iterator entry_it = m_entriesToUpdate.begin();
		for (; entry_it != m_entriesToUpdate.end(); ++entry_it) {
			Entry* entry = m_entries[*entry_it];
			entry->forceUpdate = false;
			if (entry->instanceIndex == -1) {
				entry->updateInfo = EntryNoneUpdate;
				removes.insert(*entry_it);
				continue;
			}
			RenderItem* item = m_renderItems[entry->instanceIndex];
			bool onScreenA = entry->visible && item->image && item->dimensions.intersects(viewport);
			bool positionUpdate = (entry->updateInfo & EntryPositionUpdate) == EntryPositionUpdate;
			if ((entry->updateInfo & EntryVisualUpdate) == EntryVisualUpdate) {
				positionUpdate |= updateVisual(entry);
			}
			if (positionUpdate) {
				updatePosition(entry);
			}
			bool onScreenB = entry->visible && item->image && item->dimensions.intersects(viewport);
			if (onScreenA != onScreenB) {
				if (!onScreenA) {
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
			} else if (onScreenA && onScreenB && positionUpdate) {
				// sort
				needSorting.push_back(item);
			}

			if (!entry->forceUpdate) {
				entry->forceUpdate = false;
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
		RenderItem* item = m_renderItems[entry->instanceIndex];
		Instance* instance = item->instance;
		InstanceVisual* visual = instance->getVisual<InstanceVisual>();
		item->facingAngle = instance->getRotation();
		int32_t angle = static_cast<int32_t>(m_camera->getRotation()) + item->facingAngle;
		Action* action = instance->getCurrentAction();
		ImagePtr image;

		if (visual) {
			uint8_t layerTrans = m_layer->getLayerTransparency();
			uint8_t instanceTrans = visual->getTransparency();
			if (layerTrans != 0) {
				if (instanceTrans != 0) {
					uint8_t calcTrans = layerTrans - instanceTrans;
					if (calcTrans >= 0) {
						instanceTrans = calcTrans;
					} else {
						instanceTrans = 0;
					}
				} else {
					instanceTrans = layerTrans;
				}
			}
			item->transparency = 255 - instanceTrans;
			// only visible if visual is visible and item is not totally transparent
			entry->visible = (visual->isVisible() && item->transparency != 0);
		}
		// delete old overlay
		item->deleteOverlayData();

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
		entry->forceUpdate = (action != 0);

		if (action) {
			ActionVisual* actionVisual = action->getVisual<ActionVisual>();
			bool colorOverlay = actionVisual->isColorOverlay();
			// assumed all have the same size
			if (actionVisual->isAnimationOverlay()) {
				std::map<int32_t, AnimationPtr> animations = actionVisual->getAnimationOverlay(angle);
				std::map<int32_t, AnimationPtr>::iterator it = animations.begin();
				std::vector<ImagePtr>* animOverlays = new std::vector<ImagePtr>();
				std::vector<OverlayColors*>* animationColorOverlays = colorOverlay ? new std::vector<OverlayColors*>() : 0;
				for (; it != animations.end(); ++it) {
					uint32_t animationTime = instance->getActionRuntime() % it->second->getDuration();
					image = it->second->getFrameByTimestamp(animationTime);
					animOverlays->push_back(image);
					
					if (colorOverlay) {
						OverlayColors* co = actionVisual->getColorOverlay(angle, it->first);
						if (co) {
							AnimationPtr ovAnim = co->getColorOverlayAnimation();
							animationTime = instance->getActionRuntime() % ovAnim->getDuration();
							co->setColorOverlayImage(ovAnim->getFrameByTimestamp(animationTime));
						}
						animationColorOverlays->push_back(co);
					}
					// works only for one animation
					int32_t actionFrame = it->second->getActionFrame();
					if (actionFrame != -1) {
						int32_t newFrame = it->second->getFrameIndex(animationTime);
						if (item->currentFrame != newFrame) {
							if (actionFrame == newFrame) {
								instance->callOnActionFrame(action, actionFrame);
							// if action frame was skipped
							} else if (newFrame > actionFrame && item->currentFrame < actionFrame) {
								instance->callOnActionFrame(action, actionFrame);
							}
							item->currentFrame = newFrame;
						}
					}
				}
				// transfer ownership of the vectors to RenderItem
				item->setAnimationOverlay(animOverlays, animationColorOverlays);
			} else {
				AnimationPtr animation = action->getVisual<ActionVisual>()->getAnimationByAngle(angle);
				uint32_t animationTime = instance->getActionRuntime() % animation->getDuration();
				image = animation->getFrameByTimestamp(animationTime);
				// if the action have an animation with only one frame (idle animation) then
				// a forced update is not necessary.
				if (animation->getFrameCount() <= 1) {
					entry->forceUpdate = false;
				}
				if (colorOverlay) {
					OverlayColors* co = actionVisual->getColorOverlay(angle);
					if (co) {
						AnimationPtr ovAnim = co->getColorOverlayAnimation();
						animationTime = instance->getActionRuntime() % ovAnim->getDuration();
						co->setColorOverlayImage(ovAnim->getFrameByTimestamp(animationTime));
						item->setColorOverlay(co);
					}
				}
				int32_t actionFrame = animation->getActionFrame();
				if (actionFrame != -1) {
					if (item->image != image) {
						int32_t newFrame = animation->getFrameIndex(animationTime);
						if (actionFrame == newFrame) {
							instance->callOnActionFrame(action, actionFrame);
						// if action frame was skipped
						} else if (newFrame > actionFrame && item->currentFrame < actionFrame) {
							instance->callOnActionFrame(action, actionFrame);
						}
						item->currentFrame = newFrame;
					}
				}
			}
		}

		bool newPosition = false;
		if (image != item->image) {
			if (!item->image || !image) {
				newPosition = true;
			} else if (image->getWidth() != item->image->getWidth() ||
						image->getHeight() != item->image->getHeight() ||
						image->getXShift() != item->image->getXShift() ||
						image->getYShift() != item->image->getYShift()) {
							newPosition = true;
			}
			item->image = image;
		}
		return newPosition;
	}

	void LayerCache::updatePosition(Entry* entry) {
		RenderItem* item = m_renderItems[entry->instanceIndex];
		Instance* instance = item->instance;
		ExactModelCoordinate mapCoords = instance->getLocationRef().getMapCoordinates();
		DoublePoint3D screenPosition = m_camera->toVirtualScreenCoordinates(mapCoords);
		ImagePtr image = item->image;

		if (image) {
			int32_t w = image->getWidth();
			int32_t h = image->getHeight();
			screenPosition.x = (screenPosition.x - w / 2) + image->getXShift();
			screenPosition.y = (screenPosition.y - h / 2) + image->getYShift();
			item->bbox.w = w;
			item->bbox.h = h;
		} else {
			item->bbox.w = 0;
			item->bbox.h = 0;
		}
		item->screenpoint = screenPosition;
		item->bbox.x = static_cast<int32_t>(screenPosition.x);
		item->bbox.y = static_cast<int32_t>(screenPosition.y);

		updateScreenCoordinate(item);

		CacheTree::Node* node = m_tree->find_container(item->bbox);
		if (node) {
			if (node != entry->node) {
				if (entry->node) {
					entry->node->data().erase(entry->entryIndex);
				}
				entry->node = node;
				node->data().insert(entry->entryIndex);
			}
		}
	}

	inline void LayerCache::updateScreenCoordinate(RenderItem* item, bool changedZoom) {
		Point3D screenPoint = m_camera->virtualScreenToScreen(item->screenpoint);
		// NOTE:
		// One would expect this to be necessary here,
		// however it works the same without, sofar
		// m_camera->calculateZValue(screenPoint);
		// item->screenpoint.z = -screenPoint.z;
		item->dimensions.x = screenPoint.x;
		item->dimensions.y = screenPoint.y;

		if (changedZoom) {
			if (m_zoomed) {
				item->dimensions.w = round(static_cast<double>(item->bbox.w) * m_zoom);
				item->dimensions.h = round(static_cast<double>(item->bbox.h) * m_zoom);
			} else {
				item->dimensions.w = item->bbox.w;
				item->dimensions.h = item->bbox.h;
			}
		}
	}

	void LayerCache::sortRenderList(RenderList& renderlist) {
		if (renderlist.empty()) {
			return;
		}
		// We want to put every z value in layer z range.
		// To do it, we simply solve
		// { y1 = a*x1 + b
		// { y2 = a*x2 + b
		// where [y1,y2]' = layer z offset min/max is required z range,
		// and [x1,x2]' is expected min,max z coords.

		// more an workaround, because z values are wrong in case of inverted top with bottom
		if (!m_needSorting && !m_layer->isStatic()) {
		//if (!m_needSorting) {
			float det = m_zMin - m_zMax;
			if (fabs(det) > FLT_EPSILON) {
				static const float globalrange = 200.0;
				static const float stackdelta = (FLT_EPSILON * 100.0);
				int32_t numlayers = m_layer->getLayerCount();
				float lmin = m_layer->getZOffset();
				float lmax = lmin + globalrange/numlayers;
				float a = (lmin - lmax) / det;
				float b = (lmax * m_zMin - lmin * m_zMax) / det;

				RenderList::iterator it = renderlist.begin();
				for ( ; it != renderlist.end(); ++it) {
					InstanceVisual* vis = (*it)->instance->getVisual<InstanceVisual>();
					float& z = (*it)->vertexZ;
					z = (a * (*it)->screenpoint.z + b) + vis->getStackPosition() * stackdelta;
				}
			}
		} else {
			SortingStrategy strat = m_layer->getSortingStrategy();
			switch (strat) {
				case SORTING_CAMERA: {
					InstanceDistanceSortCamera ids;
					std::stable_sort(renderlist.begin(), renderlist.end(), ids);
				} break;
				case SORTING_LOCATION: {
					InstanceDistanceSortLocation ids(m_camera->getRotation());
					std::stable_sort(renderlist.begin(), renderlist.end(), ids);
				} break;
				case SORTING_CAMERA_AND_LOCATION: {
					InstanceDistanceSortCameraAndLocation ids;
					std::stable_sort(renderlist.begin(), renderlist.end(), ids);
				} break;
				default: {
					InstanceDistanceSortCamera ids;
					std::stable_sort(renderlist.begin(), renderlist.end(), ids);
				} break;
			}
		}
	}

	ImagePtr LayerCache::getCacheImage() {
		return m_cacheImage;
	}

	void LayerCache::setCacheImage(ImagePtr image) {
		m_cacheImage = image;
	}
}
