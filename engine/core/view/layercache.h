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
		typedef QuadTree<std::set<int> > CacheTree;

		LayerCache(Camera* camera, ImagePool* image_pool, AnimationPool* animation_pool);
		~LayerCache();

		void setLayer(Layer* layer);
		void update(Camera::Transform transform, RenderList& renderlist);

		void addInstance(Instance* instance);
		void removeInstance(Instance* instance);

		void updateInstance(Instance* instance);


	private:
		void collect(const Rect& viewport, std::vector<int>& indices);
		void reset();
		void fullUpdate();

		struct Entry {
			/// Node in m_tree;
			CacheTree::Node* node;

			/// Index in m_instances;
			unsigned instance_index;

			/// Index in m_entries;
			unsigned entry_index;
			/// Force update for entries with animation
			bool force_update;
		};

		ImagePool* m_image_pool;
		AnimationPool* m_animation_pool;
		Camera* m_camera;
		Layer* m_layer;
		CacheLayerChangeListener* m_layer_observer;

		void updateEntry(Entry& item);

		std::map<Instance*,int> m_instance_map;
		std::vector<Entry> m_entries;

		CacheTree* m_tree;
		std::vector<RenderItem> m_instances;
	};

}
#endif
