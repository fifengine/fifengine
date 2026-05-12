// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_RENDERERNODE_H
#define FIFE_RENDERERNODE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// #include <vector>

// 3rd party library includes

// FIFE includes
#include "model/structures/location.h"
#include "util/structures/point.h"

namespace FIFE
{

    class Camera;
    class Instance;
    class InstanceDeleteListener;
    class Layer;
    class Location;

    class FIFE_API RendererNode
    {
        public:
            RendererNode(
                Instance* attached_instance,
                Location const & relative_location,
                Layer* relative_layer,
                Point const & relative_point = Point(0, 0));
            RendererNode(
                Instance* attached_instance,
                Location const & relative_location,
                Point const & relative_point = Point(0, 0));
            RendererNode(
                Instance* attached_instance, Layer* relative_layer, Point const & relative_point = Point(0, 0));
            explicit RendererNode(Instance* attached_instance, Point const & relative_point = Point(0, 0));
            RendererNode(
                Location const & attached_location, Layer* relative_layer, Point const & relative_point = Point(0, 0));
            explicit RendererNode(Location const & attached_location, Point const & relative_point = Point(0, 0));
            explicit RendererNode(Layer* attached_layer, Point const & relative_point = Point(0, 0));
            explicit RendererNode(Point const & attached_point);
            RendererNode(RendererNode const & old);
            RendererNode& operator=(RendererNode const & source);
            ~RendererNode();

            void setAttached(
                Instance* attached_instance, Location const & relative_location, Point const & relative_point);
            void setAttached(Instance* attached_instance, Location const & relative_location);
            void setAttached(Instance* attached_instance, Point const & relative_point);
            void setAttached(Instance* attached_instance);
            void setAttached(Location const & attached_location, Point const & relative_point);
            void setAttached(Location const & attached_location);
            void setAttached(Layer* attached_layer);
            void setAttached(Point const & attached_point);

            void setRelative(Location const & relative_location);
            void setRelative(Location const & relative_location, Point const & relative_point);
            void setRelative(Point const & relative_point);

            Instance* getAttachedInstance();
            Location getAttachedLocation();
            Layer* getAttachedLayer();
            Point getAttachedPoint();

            Location getOffsetLocation();
            Point getOffsetPoint();

            Instance* getInstance();
            Location getLocation();
            Location const & getLocationRef();
            Layer* getLayer();
            Point getPoint();
            Point const & getPointRef();

            void addInstance(Instance* instance);
            void changeInstance(Instance* instance);
            void removeInstance(Instance const * instance, bool listener = true);

            Point getCalculatedPoint(Camera* cam, Layer* layer, bool zoomed = false);

        private:
            void checkDeleteListener();

            Instance* m_instance;
            Location m_location;
            Layer* m_layer;
            Point m_point;
            InstanceDeleteListener* m_listener;
    };
} // namespace FIFE

#endif
