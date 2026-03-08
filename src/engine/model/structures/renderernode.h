// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_RENDERERNODE_H
#define FIFE_RENDERERNODE_H

// Standard C++ library includes
// #include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE
{

    class Camera;
    class InstanceDeleteListener;

    class RendererNode
    {
    public:
        RendererNode(
            Instance* attached_instance,
            const Location& relative_location,
            Layer* relative_layer,
            const Point& relative_point = Point(0, 0));
        RendererNode(
            Instance* attached_instance, const Location& relative_location, const Point& relative_point = Point(0, 0));
        RendererNode(Instance* attached_instance, Layer* relative_layer, const Point& relative_point = Point(0, 0));
        RendererNode(Instance* attached_instance, const Point& relative_point = Point(0, 0));
        RendererNode(
            const Location& attached_location, Layer* relative_layer, const Point& relative_point = Point(0, 0));
        RendererNode(const Location& attached_location, const Point& relative_point = Point(0, 0));
        RendererNode(Layer* attached_layer, const Point& relative_point = Point(0, 0));
        RendererNode(const Point& attached_point);
        RendererNode(const RendererNode& old);
        RendererNode& operator=(const RendererNode& source);
        ~RendererNode();

        void setAttached(Instance* attached_instance, const Location& relative_location, const Point& relative_point);
        void setAttached(Instance* attached_instance, const Location& relative_location);
        void setAttached(Instance* attached_instance, const Point& relative_point);
        void setAttached(Instance* attached_instance);
        void setAttached(const Location& attached_location, const Point& relative_point);
        void setAttached(const Location& attached_location);
        void setAttached(Layer* attached_layer);
        void setAttached(const Point& attached_point);

        void setRelative(const Location& relative_location);
        void setRelative(const Location& relative_location, Point relative_point);
        void setRelative(const Point& relative_point);

        Instance* getAttachedInstance();
        Location getAttachedLocation();
        Layer* getAttachedLayer();
        Point getAttachedPoint();

        Location getOffsetLocation();
        Point getOffsetPoint();

        Instance* getInstance();
        Location getLocation();
        const Location& getLocationRef();
        Layer* getLayer();
        Point getPoint();
        const Point& getPointRef();

        void addInstance(Instance* instance);
        void changeInstance(Instance* instance);
        void removeInstance(Instance* instance, bool listener = true);

        Point getCalculatedPoint(Camera* cam, Layer* layer, const bool zoomed = false);

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