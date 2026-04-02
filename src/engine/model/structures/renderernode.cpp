// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"
#include "video/renderbackend.h"
#include "view/camera.h"

#include "renderernode.h"

namespace FIFE
{
    static Logger _log(LM_VIEWVIEW);

    class NodeInstanceDeleteListener : public InstanceDeleteListener
    {
    public:
        explicit NodeInstanceDeleteListener(RendererNode* node) : m_node(node) { }
        ~NodeInstanceDeleteListener() override = default;

        void onInstanceDeleted(Instance* instance) override
        {
            m_node->removeInstance(instance, false);
        }

    private:
        RendererNode* m_node;
    };

    RendererNode::RendererNode(
        Instance* attached_instance,
        const Location& relative_location,
        Layer* relative_layer,
        const Point& relative_point) :
        m_instance(nullptr),
        m_location(relative_location),
        m_layer(relative_layer),
        m_point(relative_point),
        m_listener(nullptr)
    {
        addInstance(attached_instance);
    }
    RendererNode::RendererNode(
        Instance* attached_instance, const Location& relative_location, const Point& relative_point) :
        m_instance(nullptr),
        m_location(relative_location),
        m_layer(nullptr),
        m_point(relative_point),
        m_listener(nullptr)
    {
        addInstance(attached_instance);
    }
    RendererNode::RendererNode(Instance* attached_instance, Layer* relative_layer, const Point& relative_point) :
        m_instance(nullptr), m_layer(relative_layer), m_point(relative_point), m_listener(nullptr)
    {
        addInstance(attached_instance);
    }
    RendererNode::RendererNode(Instance* attached_instance, const Point& relative_point) :
        m_instance(nullptr), m_layer(nullptr), m_point(relative_point), m_listener(nullptr)
    {
        addInstance(attached_instance);
    }
    RendererNode::RendererNode(const Location& attached_location, Layer* relative_layer, const Point& relative_point) :
        m_instance(nullptr),
        m_location(attached_location),
        m_layer(relative_layer),
        m_point(relative_point),
        m_listener(nullptr)
    {
    }
    RendererNode::RendererNode(const Location& attached_location, const Point& relative_point) :
        m_instance(nullptr),
        m_location(attached_location),
        m_layer(nullptr),
        m_point(relative_point),
        m_listener(nullptr)
    {
    }
    RendererNode::RendererNode(Layer* attached_layer, const Point& relative_point) :
        m_instance(nullptr), m_layer(attached_layer), m_point(relative_point), m_listener(nullptr)
    {
    }
    RendererNode::RendererNode(const Point& attached_point) :
        m_instance(nullptr), m_layer(nullptr), m_point(attached_point), m_listener(nullptr)
    {
    }
    RendererNode::RendererNode(const RendererNode& old) :
        m_instance(nullptr), m_location(old.m_location), m_layer(old.m_layer), m_point(old.m_point), m_listener(nullptr)
    {
        addInstance(old.m_instance);
    }
    RendererNode& RendererNode::operator=(const RendererNode& source)
    {
        if (this != &source) {
            changeInstance(source.m_instance);
            m_location = source.m_location;
            m_layer    = source.m_layer;
            m_point    = source.m_point;
            m_listener = source.m_listener != nullptr ? source.m_listener : new NodeInstanceDeleteListener(this);
        }
        return *this;
    }
    RendererNode::~RendererNode()
    {
        removeInstance(m_instance);
        delete m_listener;
    }

    void RendererNode::setAttached(
        Instance* attached_instance, const Location& relative_location, const Point& relative_point)
    {
        changeInstance(attached_instance);
        m_location = relative_location;
        m_point    = relative_point;
    }
    void RendererNode::setAttached(Instance* attached_instance, const Location& relative_location)
    {
        changeInstance(attached_instance);
        m_location = relative_location;
    }
    void RendererNode::setAttached(Instance* attached_instance, const Point& relative_point)
    {
        changeInstance(attached_instance);
        m_point = relative_point;
    }
    void RendererNode::setAttached(Instance* attached_instance)
    {
        changeInstance(attached_instance);
    }
    void RendererNode::setAttached(const Location& attached_location, const Point& relative_point)
    {
        changeInstance(nullptr);
        m_location = attached_location;
        m_point    = relative_point;
    }
    void RendererNode::setAttached(const Location& attached_location)
    {
        changeInstance(nullptr);
        m_location = attached_location;
    }
    void RendererNode::setAttached(Layer* attached_layer)
    {
        m_layer = attached_layer;
    }
    void RendererNode::setAttached(const Point& attached_point)
    {
        changeInstance(nullptr);
        m_location.reset();
        m_point = attached_point;
    }

    void RendererNode::setRelative(const Location& relative_location)
    {
        if (m_instance == nullptr) {
            FL_WARN(_log, LMsg("RendererNode::setRelative(Location) - ") << "No instance attached.");
        }
        m_location = relative_location;
    }
    void RendererNode::setRelative(const Location& relative_location, const Point& relative_point)
    {
        if (m_instance == nullptr) {
            FL_WARN(_log, LMsg("RendererNode::setRelative(Location, Point) - ") << "No instance attached.");
        }
        m_location = relative_location;
        m_point    = relative_point;
    }
    void RendererNode::setRelative(const Point& relative_point)
    {
        if (m_instance == nullptr || !m_location.isValid()) {
            FL_WARN(_log, LMsg("RendererNode::setRelative(Point) - ") << "No instance or location attached.");
        }
        m_point = relative_point;
    }

    Instance* RendererNode::getAttachedInstance()
    {
        if (m_instance == nullptr) {
            FL_WARN(_log, LMsg("RendererNode::getAttachedInstance() - ") << "No instance attached.");
        }
        return m_instance;
    }
    Location RendererNode::getAttachedLocation()
    {
        if (m_instance != nullptr || !m_location.isValid()) {
            FL_WARN(_log, LMsg("RendererNode::getAttachedLocation() - ") << "No location attached.");
        }
        return m_location;
    }
    Layer* RendererNode::getAttachedLayer()
    {
        if (m_layer == nullptr) {
            FL_WARN(_log, LMsg("RendererNode::getAttachedLayer() - ") << "No layer attached.");
        }
        return m_layer;
    }
    Point RendererNode::getAttachedPoint()
    {
        if (m_instance != nullptr || m_location.isValid()) {
            FL_WARN(_log, LMsg("RendererNode::getAttachedPoint() - ") << "No point attached.");
        }
        return m_point;
    }

    Location RendererNode::getOffsetLocation()
    {
        if (m_instance == nullptr || !m_location.isValid()) {
            FL_WARN(_log, LMsg("RendererNode::getOffsetLocation() - ") << "No location as offset used.");
        }
        return m_location;
    }
    Point RendererNode::getOffsetPoint()
    {
        if (m_instance == nullptr && !m_location.isValid()) {
            FL_WARN(_log, LMsg("RendererNode::getOffsetPoint() - ") << "No point as offset used.");
        }
        return m_point;
    }

    Instance* RendererNode::getInstance()
    {
        return m_instance;
    }
    Location RendererNode::getLocation()
    {
        return m_location;
    }
    const Location& RendererNode::getLocationRef()
    {
        return m_location;
    }
    Layer* RendererNode::getLayer()
    {
        return m_layer;
    }
    Point RendererNode::getPoint()
    {
        return m_point;
    }
    const Point& RendererNode::getPointRef()
    {
        return m_point;
    }

    void RendererNode::addInstance(Instance* instance)
    {
        checkDeleteListener();
        m_instance = instance;
        if (m_instance != nullptr) {
            m_instance->addDeleteListener(m_listener);
        }
    }

    void RendererNode::changeInstance(Instance* instance)
    {
        if (m_instance == instance) {
            return;
        }
        checkDeleteListener();
        if (m_instance != nullptr) {
            m_instance->removeDeleteListener(m_listener);
        }
        m_instance = instance;
        if (m_instance != nullptr) {
            m_instance->addDeleteListener(m_listener);
        }
    }

    void RendererNode::removeInstance(const Instance* instance, bool listener)
    {
        if (m_instance == instance && (instance != nullptr)) {
            if (listener) {
                m_instance->removeDeleteListener(m_listener);
            }
            m_instance = nullptr;
        }
    }

    void RendererNode::checkDeleteListener()
    {
        if (m_listener != nullptr) {
            return;
        }
        m_listener = new NodeInstanceDeleteListener(this);
    }

    Point RendererNode::getCalculatedPoint(Camera* cam, Layer* layer, const bool zoomed)
    {
        ScreenPoint p;
        if (m_instance != nullptr) {
            if (m_layer == nullptr) {
                m_layer = m_instance->getLocationRef().getLayer();
            }
            if (m_location.isValid()) {
                p = cam->toScreenCoordinates(
                    m_instance->getLocationRef().getMapCoordinates() + m_location.getMapCoordinates());
            } else {
                p = cam->toScreenCoordinates(m_instance->getLocationRef().getMapCoordinates());
            }
        } else if (m_location.isValid()) {
            if (m_layer == nullptr) {
                m_layer = m_location.getLayer();
            }
            p = cam->toScreenCoordinates(m_location.getMapCoordinates());
        } else if (m_layer == nullptr) {
            // FIXME
            FL_WARN(
                _log,
                LMsg("RendererNode::getCalculatedPoint(Camera, Layer) - ")
                    << "No layer attached. So we use the first active layer of the renderer.");
            setAttached(layer);
        }
        if (zoomed) {
            return Point(round(m_point.x * cam->getZoom()) + p.x, round(m_point.y * cam->getZoom()) + p.y);
        }
        return Point(m_point.x + p.x, m_point.y + p.y);
    }
} // namespace FIFE
