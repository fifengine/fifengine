/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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

#ifndef FIFE_VIEW_LAYERCACHE_H
#define FIFE_VIEW_LAYERCACHE_H

// Standard C++ library includes
#include <string>
#include <map>
#include <set>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/location.h"
#include "util/math/matrix.h"
#include "util/structures/rect.h"
#include "util/structures/quadtree.h"
#include "model/metamodel/grids/cellgrid.h"

#include "rendererbase.h"

namespace FIFE {

	class Camera;
	class CacheLayerChangeListener;

	class LayerCache {
	public:
		typedef QuadTree<std::set<int32_t> > CacheTree;

		LayerCache(Camera* camera);
		~LayerCache();

		void setLayer(Layer* layer);

		void update(Camera::Transform transform, RenderList& renderlist);

		void addInstance(Instance* instance);
		void removeInstance(Instance* instance);
		void updateInstance(Instance* instance);
		
		ImagePtr getCacheImage();
		void setCacheImage(ImagePtr image);

	private:
		enum RenderEntryUpdateType {
			EntryNoneUpdate = 0x00,
			EntryVisualUpdate = 0x01,
			EntryPositionUpdate = 0x02,
			EntryFullUpdate = 0x03
		};
		typedef uint8_t RenderEntryUpdate;

		struct Entry {
			// Node in m_tree;
			CacheTree::Node* node;
			// Index in m_renderItems;
			int32_t instanceIndex;
			// Index in m_entries;
			int32_t entryIndex;
			// Force update for entries with animation
			bool forceUpdate;
			// Is visible
			bool visible;
			// Update info
			RenderEntryUpdate updateInfo;
		};

		void collect(const Rect& viewport, std::vector<int32_t>& indices);
		void reset();
		void fullUpdate(Camera::Transform transform);
		void fullCoordinateUpdate(Camera::Transform transform);
		void updateEntries(std::set<int32_t>& removes, RenderList& renderlist);
		bool updateVisual(Entry* entry);
		void updatePosition(Entry* entry);
		void updateScreenCoordinate(RenderItem* item, bool changedZoom = true);
		void sortRenderList(RenderList& renderlist);

		Camera* m_camera;
		Layer* m_layer;
		CacheLayerChangeListener* m_layerObserver;
		CacheTree* m_tree;
		ImagePtr m_cacheImage;

		std::map<Instance*, int32_t> m_instance_map;
		std::vector<Entry*> m_entries;
		std::vector<RenderItem*> m_renderItems;
		std::set<int32_t> m_entriesToUpdate;
		std::deque<int32_t> m_freeEntries;

		bool m_needSorting;
		double m_zMin;
		double m_zMax;

		double m_zoom;
		bool m_zoomed;
		bool m_straightZoom;
	};
}
#endif
