// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIEW_LAYERCACHE_H
#define FIFE_VIEW_LAYERCACHE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <deque>
#include <map>
#include <set>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/location.h"
#include "util/math/matrix.h"
#include "util/structures/quadtree.h"
#include "util/structures/rect.h"

#include "rendererbase.h"

namespace FIFE
{

    class Camera;
    class CacheLayerChangeListener;

    class /*FIFE_API*/ LayerCache
    {
    public:
        using CacheTree = QuadTree<std::set<int32_t>>;
        explicit LayerCache(Camera* camera);
        ~LayerCache();

        void setLayer(Layer* layer);

        void update(Camera::Transform transform, RenderList& renderlist);

        void addInstance(Instance* instance);
        void removeInstance(Instance* instance);
        void updateInstance(Instance* instance);

        ImagePtr getCacheImage();
        void setCacheImage(const ImagePtr& image);

    private:
        enum RenderEntryUpdateType : uint8_t
        {
            EntryNoneUpdate     = 0x00,
            EntryVisualUpdate   = 0x01,
            EntryPositionUpdate = 0x02,
            EntryFullUpdate     = 0x03
        };
        using RenderEntryUpdate = uint8_t;
        struct /*FIFE_API*/ Entry
        {
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

        void collect(const Rect& viewport, std::vector<int32_t>& index_list);
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
} // namespace FIFE
#endif
