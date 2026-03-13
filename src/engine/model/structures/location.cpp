// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/grids/cellgrid.h"
#include "util/base/exception.h"

#include "layer.h"
#include "location.h"

namespace FIFE
{
    static std::string INVALID_LAYER_SET = "Cannot set layer coordinates, given layer is not initialized properly";
    static std::string INVALID_LAYER_GET = "Cannot get layer coordinates, layer is not initialized properly";

    Location::Location() : m_layer(nullptr)
    {
        reset();
    }

    Location::Location(const Location& loc) : m_layer(nullptr)
    {
        reset();
        m_layer              = loc.m_layer;
        m_exact_layer_coords = loc.m_exact_layer_coords;
    }

    Location::Location(Layer* layer) : m_layer(nullptr)
    {
        reset();
        m_layer = layer;
    }

    Location::~Location() = default;

    void Location::reset()
    {
        m_exact_layer_coords.x = 0;
        m_exact_layer_coords.y = 0;
        m_exact_layer_coords.z = 0;
        m_layer                = nullptr;
    }

    Location& Location::operator=(const Location& rhs)
    {
        if (this == &rhs) {
            return *this;
        }
        m_layer                = rhs.m_layer;
        m_exact_layer_coords.x = rhs.m_exact_layer_coords.x;
        m_exact_layer_coords.y = rhs.m_exact_layer_coords.y;
        m_exact_layer_coords.z = rhs.m_exact_layer_coords.z;
        return *this;
    }

    Map* Location::getMap() const
    {
        if (m_layer == nullptr) {
            return nullptr;
        }
        return m_layer->getMap();
    }

    void Location::setLayer(Layer* layer)
    {
        m_layer = layer;
    }

    Layer* Location::getLayer() const
    {
        return m_layer;
    }

    void Location::setExactLayerCoordinates(const ExactModelCoordinate& coordinates)
    {
        if (!isValid()) {
            throw NotSet(INVALID_LAYER_SET);
        }
        m_exact_layer_coords = coordinates;
    }

    void Location::setLayerCoordinates(const ModelCoordinate& coordinates)
    {
        setExactLayerCoordinates(intPt2doublePt(coordinates));
    }

    void Location::setMapCoordinates(const ExactModelCoordinate& coordinates)
    {
        if (!isValid()) {
            throw NotSet(INVALID_LAYER_SET);
        }
        m_exact_layer_coords = m_layer->getCellGrid()->toExactLayerCoordinates(coordinates);
    }

    ExactModelCoordinate& Location::getExactLayerCoordinatesRef()
    {
        return m_exact_layer_coords;
    }

    ExactModelCoordinate Location::getExactLayerCoordinates() const
    {
        return m_exact_layer_coords;
    }

    ModelCoordinate Location::getLayerCoordinates() const
    {
        return m_layer->getCellGrid()->toLayerCoordinatesFromExactLayerCoordinates(m_exact_layer_coords);
    }

    ExactModelCoordinate Location::getMapCoordinates() const
    {
        return m_layer->getCellGrid()->toMapCoordinates(m_exact_layer_coords);
    }

    bool Location::isValid() const
    {
        return isValid(m_layer);
    }

    bool Location::isValid(const Layer* layer) const
    {
        return ((layer != nullptr) && (layer->getCellGrid() != nullptr));
    }

    ExactModelCoordinate Location::getExactLayerCoordinates(const Layer* layer) const
    {
        if (!isValid(layer)) {
            throw NotSet(INVALID_LAYER_GET);
        }

        if (layer == m_layer) {
            return m_exact_layer_coords;
        }

        CellGrid* cg1 = m_layer->getCellGrid();
        CellGrid* cg2 = layer->getCellGrid();
        return cg2->toExactLayerCoordinates(cg1->toMapCoordinates(m_exact_layer_coords));
    }

    ModelCoordinate Location::getLayerCoordinates(const Layer* layer) const
    {
        if (!isValid(layer)) {
            throw NotSet(INVALID_LAYER_GET);
        }

        if (layer == m_layer) {
            return getLayerCoordinates();
        }

        CellGrid* cg1 = m_layer->getCellGrid();
        CellGrid* cg2 = layer->getCellGrid();
        return cg2->toLayerCoordinates(cg1->toMapCoordinates(m_exact_layer_coords));
    }

    double Location::getCellOffsetDistance() const
    {
        const ExactModelCoordinate& pt = m_exact_layer_coords;
        double dx                      = pt.x - static_cast<double>(static_cast<int32_t>(pt.x));
        double dy                      = pt.y - static_cast<double>(static_cast<int32_t>(pt.y));
        return Mathd::Sqrt((dx * dx) + (dy * dy));
    }

    std::ostream& operator<<(std::ostream& os, const Location& l)
    {
        ExactModelCoordinate p = l.getExactLayerCoordinates();
        return os << "x=" << p.x << ", y=" << p.y;
    }

    double Location::getMapDistanceTo(const Location& location) const
    {
        ExactModelCoordinate current = getMapCoordinates();
        ExactModelCoordinate target  = location.getMapCoordinates();

        double rx = current.x - target.x;
        double ry = current.y - target.y;
        double rz = current.z - target.z;

        return Mathd::Sqrt((rx * rx) + (ry * ry) + (rz * rz));
    }

    double Location::getLayerDistanceTo(const Location& location) const
    {
        ModelCoordinate current = getLayerCoordinates();
        ModelCoordinate target  = location.getLayerCoordinates(m_layer);

        double rx = current.x - target.x;
        double ry = current.y - target.y;
        double rz = current.z - target.z;

        return Mathd::Sqrt((rx * rx) + (ry * ry) + (rz * rz));
    }
} // namespace FIFE
