// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "model/metamodel/action.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/timeprovider.h"
#include "model/structures/instance.h"
#include "model/structures/instancetree.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "model/structures/map.h"
#include "util/log/logger.h"
#include "util/math/angles.h"
#include "util/math/fife_math.h"
#include "util/time/timemanager.h"
#include "video/animation.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"

#include "camera.h"
#include "layercache.h"
#include "visual.h"

namespace FIFE
{
    static Logger _log(LM_CAMERA);

    // to avoid std::bad_alloc errors, we determine the maximum size of batches
    const uint32_t MAX_BATCH_SIZE = 100000;

    class MapObserver : public MapChangeListener
    {
        Camera* m_camera;

    public:
        explicit MapObserver(Camera* camera)
        {
            m_camera = camera;
        }
        ~MapObserver() override = default;

        void onMapChanged(Map* map, std::vector<Layer*>& changedLayers) override { }

        void onLayerCreate(Map* map, Layer* layer) override
        {
            m_camera->addLayer(layer);
        }

        void onLayerDelete(Map* map, Layer* layer) override
        {
            m_camera->removeLayer(layer);
        }
    };

    Camera::Camera(std::string id, Map* map, const Rect& viewport, RenderBackend* renderbackend) :
        m_id(std::move(id)),
        m_map(map),
        m_viewport(viewport),
        m_renderbackend(renderbackend),
        m_position(ExactModelCoordinate(0, 0, 0)),

        m_tilt(0),
        m_rotation(0),
        m_zoom(1),
        m_zToY(0),
        m_enabledZToY(false),

        m_curOrigin(ScreenPoint(0, 0, 0)),

        m_mapViewPortUpdated(false),
        m_screenCellWidth(1),
        m_screenCellHeight(1),
        m_referenceScaleX(1),
        m_referenceScaleY(1),
        m_enabled(true),
        m_attachedTo(nullptr),
        m_transform(NoneTransform),

        m_updated(false),

        m_lighting(false),

        m_col_overlay(false),
        m_img_overlay(false),
        m_ani_overlay(false),
        m_overlay_color{0, 0, 0, 0},
        m_img_id(-1),
        m_img_fill(false),
        m_ani_fill(false),
        m_start_time(0)
    {

        m_map_observer = new MapObserver(this);
        init();
    }

    Camera::~Camera()
    {
        // Trigger removal of LayerCaches and MapObserver
        if (m_map != nullptr) {
            m_map->removeChangeListener(m_map_observer);
            const std::list<Layer*>& layers = m_map->getLayers();
            for (auto layer : layers) {
                removeLayer(layer);
            }
        }

        auto r_it = m_renderers.begin();
        for (; r_it != m_renderers.end(); ++r_it) {
            r_it->second->reset();
            delete r_it->second;
        }
        m_renderers.clear();
        delete m_map_observer;
    }

    void Camera::init()
    {
        m_transform |= PositionTransform;
        updateMatrices();

        m_curOrigin = toScreenCoordinates(m_position);

        // Trigger addition of LayerCaches and MapObserver
        m_map->addChangeListener(m_map_observer);
        const std::list<Layer*>& layers = m_map->getLayers();
        for (auto layer : layers) {
            addLayer(layer);
        }
    }

    void Camera::setTilt(double tilt)
    {
        if (!Mathd::Equal(m_tilt, tilt)) {
            m_transform |= TiltTransform;
            m_tilt = tilt;
            updateReferenceScale();
            updateMatrices();
        }
    }

    double Camera::getTilt() const
    {
        return m_tilt;
    }

    void Camera::setRotation(double rotation)
    {
        if (!Mathd::Equal(m_rotation, rotation)) {
            m_transform |= RotationTransform;
            m_rotation = rotation;
            updateReferenceScale();
            updateMatrices();
        }
    }

    double Camera::getRotation() const
    {
        return m_rotation;
    }

    void Camera::setZoom(double zoom)
    {
        if (!Mathd::Equal(m_zoom, zoom)) {
            m_transform |= ZoomTransform;
            m_zoom = zoom;
            if (m_zoom < 0.001) {
                m_zoom = 0.001;
            }
            updateMatrices();
        }
    }

    double Camera::getZoom() const
    {
        return m_zoom;
    }

    double Camera::getOriginalZToY() const
    {
        DoubleMatrix matrix;
        matrix.loadScale(m_referenceScaleX, m_referenceScaleY, m_referenceScaleX);
        if (m_location.getLayer() != nullptr) {
            CellGrid* cg = m_location.getLayer()->getCellGrid();
            if (cg != nullptr) {
                ExactModelCoordinate pt = m_location.getMapCoordinates();
                matrix.applyTranslate(-pt.x * m_referenceScaleX, -pt.y * m_referenceScaleY, -pt.z * m_referenceScaleX);
            }
        }
        matrix.applyRotate(-m_rotation, 0.0, 0.0, 1.0);
        matrix.applyRotate(-m_tilt, 1.0, 0.0, 0.0);
        return matrix.m9 * -1.0;
    }

    void Camera::setZToY(double zToY)
    {
        m_enabledZToY = true;
        if (!Mathd::Equal(m_zToY, zToY)) {
            m_transform |= ZTransform;
            m_zToY = zToY;
            updateMatrices();
        }
    }

    double Camera::getZToY() const
    {
        return m_zToY;
    }

    void Camera::setZToYEnabled(bool enabled)
    {
        m_enabledZToY = enabled;
    }

    bool Camera::isZToYEnabled() const
    {
        return m_enabledZToY;
    }

    void Camera::setCellImageDimensions(uint32_t width, uint32_t height)
    {
        m_screenCellWidth  = width;
        m_screenCellHeight = height;
        updateReferenceScale();
        updateMatrices();
        m_transform |= PositionTransform;
    }

    void Camera::setLocation(const Location& location)
    {
        if (m_location == location) {
            return;
        }

        CellGrid* cell_grid = nullptr;
        if (location.getLayer() != nullptr) {
            cell_grid = location.getLayer()->getCellGrid();
            if (cell_grid == nullptr) {
                throw Exception("Location layer without cellgrid given to Camera::setLocation");
            }
        } else {
            throw Exception("Location without layer given to Camera::setLocation");
        }

        m_transform |= PositionTransform;
        m_location = location;
        m_position = m_location.getMapCoordinates();
        updateMatrices();
        m_curOrigin = toScreenCoordinates(m_position);
    }

    Point Camera::getCellImageDimensions()
    {
        return Point(m_screenCellWidth, m_screenCellHeight);
    }

    Point Camera::getCellImageDimensions(Layer* layer)
    {
        Point p;
        DoublePoint dimensions = getLogicalCellDimensions(layer);
        p.x                    = static_cast<int32_t>(round(m_referenceScaleX * dimensions.x));
        p.y                    = static_cast<int32_t>(round(m_referenceScaleY * dimensions.y));
        return p;
    }

    Location Camera::getLocation()
    {
        if (m_location.getLayer() != nullptr) {
            m_location.setMapCoordinates(m_position);
            return m_location;
        }
        Location loc = Location();
        if ((m_map != nullptr) && m_map->getLayerCount() > 0) {
            loc.setLayer(m_map->getLayers().back());
            loc.setMapCoordinates(m_position);
        }
        return loc;
    }

    void Camera::setViewPort(const Rect& viewport)
    {
        m_viewport = viewport;
        refresh();
    }

    const Rect& Camera::getViewPort() const
    {
        return m_viewport;
    }

    const Rect& Camera::getMapViewPort()
    {
        if (!m_mapViewPortUpdated) {
            ScreenPoint sp1(m_viewport.x, m_viewport.y);
            ScreenPoint sp2(m_viewport.x, m_viewport.y + m_viewport.h);
            ScreenPoint sp3(m_viewport.x + m_viewport.w, m_viewport.y);
            ScreenPoint sp4(m_viewport.x + m_viewport.w, m_viewport.y + m_viewport.h);

            std::vector<ExactModelCoordinate> coords;
            coords.push_back(toMapCoordinates(sp2, false));
            coords.push_back(toMapCoordinates(sp3, false));
            coords.push_back(toMapCoordinates(sp4, false));

            ExactModelCoordinate emc = toMapCoordinates(sp1, false);
            ModelCoordinate min(static_cast<int32_t>(emc.x), static_cast<int32_t>(emc.y));
            ModelCoordinate max(static_cast<int32_t>(emc.x + 0.5), static_cast<int32_t>(emc.y + 0.5));
            auto it = coords.begin();
            for (; it != coords.end(); ++it) {
                min.x = std::min(min.x, static_cast<int32_t>((*it).x));
                min.y = std::min(min.y, static_cast<int32_t>((*it).y));
                max.x = std::max(max.x, static_cast<int32_t>((*it).x + 0.5));
                max.y = std::max(max.y, static_cast<int32_t>((*it).y + 0.5));
            }
            // makes the viewport a bit larger
            m_mapViewPort.x      = min.x - 1;
            m_mapViewPort.y      = min.y - 1;
            m_mapViewPort.w      = ABS(max.x - min.x) + 2;
            m_mapViewPort.h      = ABS(max.y - min.y) + 2;
            m_mapViewPortUpdated = true;
        }

        return m_mapViewPort;
    }

    Rect Camera::getLayerViewPort(Layer* layer)
    {
        Rect mapView = getMapViewPort();
        Location loc(layer);
        ExactModelCoordinate emc(mapView.x, mapView.y);
        loc.setMapCoordinates(emc);
        emc.x     = mapView.x + mapView.w;
        emc.y     = mapView.y + mapView.h;
        mapView.x = loc.getLayerCoordinates().x;
        mapView.y = loc.getLayerCoordinates().y;
        loc.setMapCoordinates(emc);
        mapView.w = ABS(loc.getLayerCoordinates().x - mapView.x);
        mapView.h = ABS(loc.getLayerCoordinates().y - mapView.y);

        return mapView;
    }

    void Camera::setEnabled(bool enabled)
    {
        m_enabled = enabled;
    }

    bool Camera::isEnabled()
    {
        return m_enabled;
    }

    void Camera::setPosition(const ExactModelCoordinate& position)
    {
        if (Mathd::Equal(m_position.x, position.x) && Mathd::Equal(m_position.y, position.y)) {
            return;
        }
        m_transform |= PositionTransform;
        m_position = position;
        updateMatrices();
        m_curOrigin = toScreenCoordinates(m_position);
    }

    ExactModelCoordinate Camera::getPosition() const
    {
        return m_position;
    }

    Point3D Camera::getOrigin() const
    {
        return m_curOrigin;
    }

    void Camera::updateMatrices()
    {
        m_matrix.loadScale(m_referenceScaleX, m_referenceScaleY, m_referenceScaleX);
        m_vs_matrix.loadScale(m_referenceScaleX, m_referenceScaleY, m_referenceScaleX);

        m_matrix.applyTranslate(
            -m_position.x * m_referenceScaleX, -m_position.y * m_referenceScaleY, -m_position.z * m_referenceScaleX);

        m_matrix.applyRotate(-m_rotation, 0.0, 0.0, 1.0);
        m_matrix.applyRotate(-m_tilt, 1.0, 0.0, 0.0);
        if (m_enabledZToY) {
            m_matrix.m9 = -m_zToY; // z -> y height in pixels
        }
        double scale = m_zoom;
        m_matrix.applyScale(scale, scale, scale);
        m_matrix.applyTranslate(+m_viewport.x + (m_viewport.w / 2.0), +m_viewport.y + (m_viewport.h / 2.0), 0);
        m_inverse_matrix = m_matrix.inverse();

        m_vs_matrix.applyRotate(-m_rotation, 0.0, 0.0, 1.0);
        m_vs_matrix.applyRotate(-m_tilt, 1.0, 0.0, 0.0);
        if (m_enabledZToY) {
            m_vs_matrix.m9 = -m_zToY; // z -> y height in pixels
        }
        m_vs_inverse_matrix = m_vs_matrix.inverse();

        // calculate the screen<->virtual screen transformation
        // this explicitly ignores the z-value.
        m_vscreen_2_screen = m_matrix;
        // NOTE: mult4by4 is an in-place modification.
        m_vscreen_2_screen.mult4by4(m_vs_inverse_matrix);
        // set the z transformation to unity
        const int32_t N = 4;
        for (int32_t i = 0; i != N; ++i) {
            m_vscreen_2_screen[(2 * N) + i] = 0;
            m_vscreen_2_screen[(i * N) + 2] = 0;
        }
        m_vscreen_2_screen[(2 * N) + 2] = 1;
        m_screen_2_vscreen              = m_vscreen_2_screen.inverse();

        m_mapViewPortUpdated = false;
        // FL_WARN(_log, LMsg("matrix: ") << m_matrix << " 1: " << m_matrix.inverse().mult4by4(m_matrix));
        // 		FL_WARN(_log, LMsg("vs2s matrix: ") << m_vscreen_2_screen << " s2vs matrix: " <<
        // m_screen_2_vscreen);
    }

    void Camera::calculateZValue(DoublePoint3D& screen_coords)
    {
        int32_t dy      = -(screen_coords.y - toScreenCoordinates(m_position).y);
        screen_coords.z = Mathd::Tan(m_tilt * (Mathd::pi() / 180.0)) * static_cast<double>(dy);
    }

    ExactModelCoordinate Camera::toMapCoordinates(ScreenPoint screen_coords, bool z_calculated)
    {
        DoublePoint3D double_screen_coords = intPt2doublePt(screen_coords);
        if (!z_calculated) {
            calculateZValue(double_screen_coords);
        }
        return m_inverse_matrix * double_screen_coords;
    }

    ScreenPoint Camera::toScreenCoordinates(const ExactModelCoordinate& elevation_coords)
    {
        ScreenPoint pt = doublePt2intPt(m_matrix * elevation_coords);
        return pt;
    }

    DoublePoint3D Camera::toVirtualScreenCoordinates(const ExactModelCoordinate& elevation_coords)
    {
        DoublePoint3D pt = (m_vs_matrix * elevation_coords);
        return pt;
    }

    ScreenPoint Camera::virtualScreenToScreen(const DoublePoint3D& p)
    {
        return doublePt2intPt(m_vscreen_2_screen * p);
    }

    DoublePoint3D Camera::screenToVirtualScreen(const ScreenPoint& p)
    {
        return m_screen_2_vscreen * intPt2doublePt(p);
    }

    DoublePoint Camera::getLogicalCellDimensions(Layer* layer)
    {
        assert(layer);
        CellGrid* cg = layer->getCellGrid();
        assert(cg);

        ModelCoordinate cell(0, 0);
        std::vector<ExactModelCoordinate> vertices;
        cg->getVertices(vertices, cell);

        DoubleMatrix mtx;
        mtx.loadRotate(m_rotation, 0.0, 0.0, 1.0);
        mtx.applyRotate(m_tilt, 1.0, 0.0, 0.0);

        double x1 = 0;
        double x2 = 0;
        double y1 = 0;
        double y2 = 0;

        for (uint32_t i = 0; i < vertices.size(); i++) {
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
        return DoublePoint(x2 - x1, y2 - y1);
    }

    DoublePoint Camera::getLogicalCellDimensions()
    {
        std::vector<ExactModelCoordinate> vertices;
        vertices.emplace_back(-0.5, -0.5);
        vertices.emplace_back(0.5, -0.5);
        vertices.emplace_back(0.5, 0.5);
        vertices.emplace_back(-0.5, 0.5);

        DoubleMatrix mtx;
        mtx.loadRotate(m_rotation, 0.0, 0.0, 1.0);
        mtx.applyRotate(m_tilt, 1.0, 0.0, 0.0);

        double x1 = 0;
        double x2 = 0;
        double y1 = 0;
        double y2 = 0;

        for (uint32_t i = 0; i < vertices.size(); i++) {
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
        return DoublePoint(x2 - x1, y2 - y1);
    }

    void Camera::updateReferenceScale()
    {
        DoublePoint dim   = getLogicalCellDimensions();
        m_referenceScaleX = static_cast<double>(m_screenCellWidth) / dim.x;
        m_referenceScaleY = static_cast<double>(m_screenCellHeight) / dim.y;

        FL_DBG(_log, "Updating reference scale");
        FL_DBG(_log, LMsg("   tilt=") << m_tilt << " rot=" << m_rotation);
        FL_DBG(_log, LMsg("   m_screenCellWidth=") << m_screenCellWidth);
        FL_DBG(_log, LMsg("   m_screenCellHeight=") << m_screenCellHeight);
        FL_DBG(_log, LMsg("   m_referenceScaleX=") << m_referenceScaleX);
        FL_DBG(_log, LMsg("   m_referenceScaleY=") << m_referenceScaleY);
    }

    RenderList& Camera::getRenderListRef(Layer* layer)
    {
        return m_layerToInstances[layer];
    }

    void Camera::getMatchingInstances(
        ScreenPoint screen_coords, Layer& layer, std::list<Instance*>& instances, uint8_t alpha)
    {
        instances.clear();
        bool zoomed        = !Mathd::Equal(m_zoom, 1.0);
        bool special_alpha = alpha != 0;

        const RenderList& layer_instances = m_layerToInstances[&layer];
        auto instance_it                  = layer_instances.end();
        while (instance_it != layer_instances.begin()) {
            --instance_it;
            Instance* i          = (*instance_it)->instance;
            const RenderItem& vc = **instance_it;
            if ((vc.dimensions.contains(Point(screen_coords.x, screen_coords.y)))) {
                if (vc.image->isSharedImage()) {
                    vc.image->forceLoadInternal();
                }
                uint8_t r = 0;
                uint8_t g = 0;
                uint8_t b = 0;
                uint8_t a = 0;
                int32_t x = screen_coords.x - vc.dimensions.x;
                int32_t y = screen_coords.y - vc.dimensions.y;
                if (zoomed) {
                    auto fx  = static_cast<double>(x);
                    auto fy  = static_cast<double>(y);
                    auto fow = static_cast<double>(vc.image->getWidth());
                    auto foh = static_cast<double>(vc.image->getHeight());
                    auto fsw = static_cast<double>(vc.dimensions.w);
                    auto fsh = static_cast<double>(vc.dimensions.h);
                    x        = static_cast<int32_t>(round(fx / fsw * fow));
                    y        = static_cast<int32_t>(round(fy / fsh * foh));
                }
                if (vc.getAnimationOverlay() != nullptr) {
                    std::vector<ImagePtr>* ao = vc.getAnimationOverlay();
                    auto it                   = ao->begin();
                    for (; it != ao->end(); ++it) {
                        if ((*it)->isSharedImage()) {
                            (*it)->forceLoadInternal();
                        }
                        (*it)->getPixelRGBA(x, y, &r, &g, &b, &a);
                        // instance is hit with mouse if not totally transparent
                        if (a == 0 || (special_alpha && a < alpha)) {
                            continue;
                        }
                        instances.push_back(i);
                        break;
                    }
                } else {
                    vc.image->getPixelRGBA(x, y, &r, &g, &b, &a);
                    // instance is hit with mouse if not totally transparent
                    if (a == 0 || (special_alpha && a < alpha)) {
                        continue;
                    }
                    instances.push_back(i);
                }
            }
        }
    }

    void Camera::getMatchingInstances(Rect screen_rect, Layer& layer, std::list<Instance*>& instances, uint8_t alpha)
    {
        instances.clear();
        bool zoomed        = !Mathd::Equal(m_zoom, 1.0);
        bool special_alpha = alpha != 0;

        const RenderList& layer_instances = m_layerToInstances[&layer];
        auto instance_it                  = layer_instances.end();
        while (instance_it != layer_instances.begin()) {
            --instance_it;
            Instance* i = (*instance_it)->instance;
            ;
            const RenderItem& vc = **instance_it;
            if ((vc.dimensions.intersects(screen_rect))) {
                if (vc.image->isSharedImage()) {
                    vc.image->forceLoadInternal();
                }
                uint8_t r                   = 0;
                uint8_t g                   = 0;
                uint8_t b                   = 0;
                uint8_t a                   = 0;
                int32_t intersection_left   = std::max(screen_rect.x, vc.dimensions.x);
                int32_t intersection_right  = std::min(screen_rect.right(), vc.dimensions.right());
                int32_t intersection_top    = std::max(screen_rect.y, vc.dimensions.y);
                int32_t intersection_bottom = std::min(screen_rect.bottom(), vc.dimensions.bottom());

                for (int32_t xx = intersection_left; xx < intersection_right; xx++) {
                    for (int32_t yy = intersection_top; yy < intersection_bottom; yy++) {
                        int32_t x = xx - vc.dimensions.x;
                        int32_t y = yy - vc.dimensions.y;
                        if (zoomed) {
                            auto fx  = static_cast<double>(x);
                            auto fy  = static_cast<double>(y);
                            auto fow = static_cast<double>(vc.image->getWidth());
                            auto foh = static_cast<double>(vc.image->getHeight());
                            auto fsw = static_cast<double>(vc.dimensions.w);
                            auto fsh = static_cast<double>(vc.dimensions.h);
                            x        = static_cast<int32_t>(round(fx / fsw * fow));
                            y        = static_cast<int32_t>(round(fy / fsh * foh));
                        }
                        if (vc.getAnimationOverlay() != nullptr) {
                            std::vector<ImagePtr>* ao = vc.getAnimationOverlay();
                            auto it                   = ao->begin();
                            for (; it != ao->end(); ++it) {
                                if ((*it)->isSharedImage()) {
                                    (*it)->forceLoadInternal();
                                }
                                (*it)->getPixelRGBA(x, y, &r, &g, &b, &a);
                                // instance is hit with mouse if not totally transparent
                                if (a == 0 || (special_alpha && a < alpha)) {
                                    continue;
                                }
                                instances.push_back(i);
                                goto found_non_transparent_pixel;
                            }
                        } else {
                            vc.image->getPixelRGBA(x, y, &r, &g, &b, &a);
                            // instance is hit with mouse if not totally transparent
                            if (a == 0 || (special_alpha && a < alpha)) {
                                continue;
                            }
                            instances.push_back(i);
                            goto found_non_transparent_pixel;
                        }
                    }
                }
found_non_transparent_pixel:;
            }
        }
    }

    void Camera::getMatchingInstances(Location& loc, std::list<Instance*>& instances, bool use_exactcoordinates)
    {
        instances.clear();
        Layer* layer = loc.getLayer();
        if (layer == nullptr) {
            return;
        }

        const RenderList& layer_instances = m_layerToInstances[layer];
        auto instance_it                  = layer_instances.end();
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

    void Camera::attach(Instance* instance)
    {
        if ((instance != nullptr) && instance != m_attachedTo) {
            m_attachedTo = instance;
        }
    }

    void Camera::detach()
    {
        m_attachedTo = nullptr;
    }

    void Camera::update()
    {
        if (m_attachedTo == nullptr) {
            return;
        }
        ExactModelCoordinate pos = m_attachedTo->getLocationRef().getMapCoordinates();
        if (Mathd::Equal(m_position.x, pos.x) && Mathd::Equal(m_position.y, pos.y)) {
            return;
        }
        m_transform |= PositionTransform;
        m_position = pos;
        updateMatrices();
        m_curOrigin = toScreenCoordinates(m_position);
    }

    void Camera::refresh()
    {
        updateMatrices();
        m_transform |= RotationTransform;
        m_curOrigin = toScreenCoordinates(m_position);
    }

    void Camera::resetUpdates()
    {
        if (m_transform == NoneTransform) {
            m_updated = false;
        } else {
            m_updated = true;
        }
        m_transform = NoneTransform;
    }

    bool pipelineSort(const RendererBase* lhs, const RendererBase* rhs)
    {
        return (lhs->getPipelinePosition() < rhs->getPipelinePosition());
    }

    void Camera::addRenderer(RendererBase* renderer)
    {
        renderer->setRendererListener(this);
        m_renderers[renderer->getName()] = renderer;
        if (renderer->isEnabled()) {
            m_pipeline.push_back(renderer);
        }
        m_pipeline.sort(pipelineSort);
    }

    void Camera::onRendererPipelinePositionChanged(RendererBase* renderer)
    {
        m_pipeline.sort(pipelineSort);
    }

    void Camera::onRendererEnabledChanged(RendererBase* renderer)
    {
        const std::string& rendererName = renderer->getName();
        assert(m_renderers.find(rendererName) != m_renderers.end());
        if (renderer->isEnabled()) {
            FL_LOG(_log, LMsg("Enabling renderer ") << renderer->getName());
            m_pipeline.push_back(renderer);
            m_pipeline.sort(pipelineSort);
        } else {
            m_pipeline.remove(renderer);
        }
    }

    RendererBase* Camera::getRenderer(const std::string& name)
    {
        return m_renderers[name];
    }

    void Camera::resetRenderers()
    {
        auto r_it = m_renderers.begin();
        for (; r_it != m_renderers.end(); ++r_it) {
            r_it->second->reset();
        }
    }

    void Camera::addLayer(Layer* layer)
    {
        m_cache[layer] = new LayerCache(this);
        m_cache[layer]->setLayer(layer);
        m_layerToInstances[layer] = RenderList();
        refresh();
    }

    void Camera::removeLayer(Layer* layer)
    {
        delete m_cache[layer];
        m_cache.erase(layer);
        m_layerToInstances.erase(layer);
        if (m_location.getLayer() == layer) {
            m_location.reset();
        }
        refresh();
    }

    void Camera::setLightingColor(float red, float green, float blue)
    {
        m_lighting = true;
        m_light_colors.clear();
        m_light_colors.push_back(red);
        m_light_colors.push_back(green);
        m_light_colors.push_back(blue);
    }

    std::vector<float> Camera::getLightingColor()
    {
        if (m_light_colors.empty()) {
            for (int32_t colors = 0; colors != 3; ++colors) {
                m_light_colors.push_back(1.0F);
            }
        }
        return m_light_colors;
    }

    void Camera::resetLightingColor()
    {
        m_lighting = false;
        m_renderbackend->resetLighting();
    }

    void Camera::setOverlayColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        m_col_overlay     = true;
        m_overlay_color.r = red;
        m_overlay_color.g = green;
        m_overlay_color.b = blue;
        m_overlay_color.a = alpha;
    }

    std::vector<uint8_t> Camera::getOverlayColor()
    {
        std::vector<uint8_t> colors;
        if (m_col_overlay) {
            colors.push_back(m_overlay_color.r);
            colors.push_back(m_overlay_color.g);
            colors.push_back(m_overlay_color.b);
            colors.push_back(m_overlay_color.a);
        } else {
            for (uint8_t cc = 0; cc != 4; ++cc) {
                colors.push_back(255);
            }
        }
        return colors;
    }

    void Camera::resetOverlayColor()
    {
        m_col_overlay = false;
    }

    void Camera::setOverlayImage(int32_t id, bool fill)
    {
        m_img_overlay = true;
        m_img_id      = id;
        m_img_fill    = fill;
    }

    int32_t Camera::getOverlayImage()
    {
        int32_t id = -1;
        if (m_img_overlay) {
            id = m_img_id;
        }
        return id;
    }

    void Camera::resetOverlayImage()
    {
        m_img_overlay = false;
        m_img_id      = -1;
    }

    void Camera::setOverlayAnimation(AnimationPtr anim, bool fill)
    {
        m_ani_overlay = true;
        m_ani_ptr     = anim;
        m_ani_fill    = fill;
        m_start_time  = 0;
    }

    AnimationPtr Camera::getOverlayAnimation()
    {
        return m_ani_ptr;
    }

    void Camera::resetOverlayAnimation()
    {
        m_ani_overlay = false;
        m_ani_ptr.reset();
    }

    void Camera::renderOverlay()
    {
        if (!m_col_overlay && !m_img_overlay && !m_ani_overlay) {
            return;
        }
        uint16_t width  = m_viewport.w;
        uint16_t height = m_viewport.h;
        Point pm        = Point(m_viewport.x + (width / 2), m_viewport.y + (height / 2));
        Rect r;

        // color overlay
        if (m_col_overlay) {
            Point p = Point(m_viewport.x, m_viewport.y);
            m_renderbackend->fillRectangle(
                p, width, height, m_overlay_color.r, m_overlay_color.g, m_overlay_color.b, m_overlay_color.a);
        }
        // image overlay
        if (m_img_overlay) {
            ImagePtr resptr = ImageManager::instance()->get(m_img_id);
            Image* img      = resptr.get();
            if (img != nullptr) {
                if (m_img_fill) {
                    r.w = width;
                    r.h = height;
                } else {
                    r.w = img->getWidth();
                    r.h = img->getHeight();
                }
                r.x = pm.x - r.w / 2;
                r.y = pm.y - r.h / 2;
                img->render(r);
            }
        }
        // animation overlay
        if (m_ani_overlay) {
            assert(m_ani_ptr != 0);

            if (m_start_time == 0) {
                m_start_time = TimeManager::instance()->getTime();
            }
            uint32_t animtime =
                scaleTime(1.0, TimeManager::instance()->getTime() - m_start_time) % m_ani_ptr->getDuration();
            ImagePtr img = m_ani_ptr->getFrameByTimestamp(animtime);
            if (img) {
                if (m_ani_fill) {
                    r.w = width;
                    r.h = height;
                } else {
                    r.w = img->getWidth();
                    r.h = img->getHeight();
                }
                r.x = pm.x - r.w / 2;
                r.y = pm.y - r.h / 2;
                img->render(r);
            }
        }
    }

    void Camera::renderStaticLayer(Layer* layer, bool update)
    {
        // ToDo: Remove this function from the camera class to something like engine pre-render.
        // ToDo: Check if partial rendering of only updated RenderItems to existing FBO is possible/faster in our case.
        // ToDo: Add and fix support for SDL backend, for SDL it works only on the lowest layer(alpha/transparent bug).
        LayerCache* cache   = m_cache[layer];
        ImagePtr cacheImage = cache->getCacheImage();
        if (cacheImage.get() == nullptr) {
            // the cacheImage name will be, camera id + _virtual_layer_image_ + layer id
            cacheImage = ImageManager::instance()->loadBlank(
                m_id + "_virtual_layer_image_" + layer->getId(), m_viewport.w, m_viewport.h);
            cache->setCacheImage(cacheImage);
            update = true;
        }
        if (update) {
            // for the case that the viewport size is not the same as the screen size,
            // we have to change the values for OpenGL backend
            Rect rec(0, m_renderbackend->getHeight() - m_viewport.h, m_viewport.w, m_viewport.h);
            if (m_renderbackend->getName() == "SDL") {
                rec = m_viewport;
            }
            m_renderbackend->attachRenderTarget(cacheImage, true);
            // here we use the new viewport size
            m_renderbackend->pushClipArea(rec, false);
            // render stuff to texture
            RenderList& instancesToRender = m_layerToInstances[layer];
            // split the RenderList into smaller parts
            if (instancesToRender.size() > MAX_BATCH_SIZE) {
                uint8_t batches   = ceil(instancesToRender.size() / static_cast<float>(MAX_BATCH_SIZE));
                uint32_t residual = instancesToRender.size() % MAX_BATCH_SIZE;
                for (uint8_t i = 0; i < batches; ++i) {
                    uint32_t start = i * MAX_BATCH_SIZE;
                    uint32_t end   = start + ((i + 1 == batches) ? residual : MAX_BATCH_SIZE);
                    RenderList tempList(instancesToRender.begin() + start, instancesToRender.begin() + end);
                    auto r_it = m_pipeline.begin();
                    for (; r_it != m_pipeline.end(); ++r_it) {
                        if ((*r_it)->isActivedLayer(layer)) {
                            (*r_it)->render(this, layer, tempList);
                            m_renderbackend->renderVertexArrays();
                        }
                    }
                }
            } else {
                auto r_it = m_pipeline.begin();
                for (; r_it != m_pipeline.end(); ++r_it) {
                    if ((*r_it)->isActivedLayer(layer)) {
                        (*r_it)->render(this, layer, instancesToRender);
                        m_renderbackend->renderVertexArrays();
                    }
                }
            }
            m_renderbackend->detachRenderTarget();
            m_renderbackend->popClipArea();
        }
    }

    void Camera::updateRenderLists()
    {
        if (m_map == nullptr) {
            FL_ERR(_log, "No map for camera found");
            return;
        }

        const std::list<Layer*>& layers = m_map->getLayers();
        auto layer_it                   = layers.begin();
        for (; layer_it != layers.end(); ++layer_it) {
            LayerCache* cache = m_cache[*layer_it];
            if (cache == nullptr) {
                addLayer(*layer_it);
                cache = m_cache[*layer_it];
                FL_ERR(_log, LMsg("Layer Cache miss! (This shouldn't happen!)") << (*layer_it)->getId());
            }
            RenderList& instancesToRender = m_layerToInstances[*layer_it];
            if ((*layer_it)->isStatic() && m_transform == NoneTransform) {
                continue;
            }
            cache->update(m_transform, instancesToRender);
        }
        resetUpdates();
    }

    void Camera::render()
    {
        updateRenderLists();

        if (m_map == nullptr) {
            return;
        }

        uint32_t lm = m_renderbackend->getLightingModel();
        if (lm != 0) {
            m_renderbackend->resetStencilBuffer(0);
            if (m_lighting) {
                m_renderbackend->setLighting(m_light_colors[0], m_light_colors[1], m_light_colors[2]);
            }
        }

        const std::list<Layer*>& layers = m_map->getLayers();
        auto layer_it                   = layers.begin();
        for (; layer_it != layers.end(); ++layer_it) {
            // layer with static flag will rendered as one texture
            if ((*layer_it)->isStatic()) {
                renderStaticLayer(*layer_it, m_updated);
                continue;
            }
        }

        m_renderbackend->pushClipArea(getViewPort());

        layer_it = layers.begin();
        for (; layer_it != layers.end(); ++layer_it) {
            // layer with static flag will rendered as one texture
            if ((*layer_it)->isStatic()) {
                m_cache[*layer_it]->getCacheImage()->render(m_viewport);
                m_renderbackend->renderVertexArrays();
                continue;
            }
            RenderList& instancesToRender = m_layerToInstances[*layer_it];
            // split the RenderList into smaller parts
            if (instancesToRender.size() > MAX_BATCH_SIZE) {
                uint8_t batches   = ceil(instancesToRender.size() / static_cast<float>(MAX_BATCH_SIZE));
                uint32_t residual = instancesToRender.size() % MAX_BATCH_SIZE;
                for (uint8_t i = 0; i < batches; ++i) {
                    uint32_t start = i * MAX_BATCH_SIZE;
                    uint32_t end   = start + ((i + 1 == batches) ? residual : MAX_BATCH_SIZE);
                    RenderList tempList(instancesToRender.begin() + start, instancesToRender.begin() + end);
                    auto r_it = m_pipeline.begin();
                    for (; r_it != m_pipeline.end(); ++r_it) {
                        if ((*r_it)->isActivedLayer(*layer_it)) {
                            (*r_it)->render(this, *layer_it, tempList);
                            m_renderbackend->renderVertexArrays();
                        }
                    }
                }
            } else {
                auto r_it = m_pipeline.begin();
                for (; r_it != m_pipeline.end(); ++r_it) {
                    if ((*r_it)->isActivedLayer(*layer_it)) {
                        (*r_it)->render(this, *layer_it, instancesToRender);
                        m_renderbackend->renderVertexArrays();
                    }
                }
            }
        }

        renderOverlay();
        m_renderbackend->renderVertexArrays();
        if (m_lighting && lm != 0) {
            m_renderbackend->resetLighting();
        }
        m_renderbackend->popClipArea();
    }
} // namespace FIFE
