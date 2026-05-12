// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "squaregrid.h"

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "util/log/logger.h"
#include "util/math/fife_math.h"

namespace FIFE
{
    static Logger _log(LM_SQUAREGRID);

    SquareGrid::SquareGrid() = default;

    CellGrid* SquareGrid::clone()
    {
        auto* nGrid = new SquareGrid();
        nGrid->setRotation(m_rotation);
        nGrid->setXScale(m_xscale);
        nGrid->setYScale(m_yscale);
        nGrid->setXShift(m_xshift);
        nGrid->setYShift(m_yshift);
        nGrid->setZShift(m_zshift);
        nGrid->setAllowDiagonals(m_allow_diagonals);

        return nGrid;
    }

    SquareGrid::~SquareGrid() = default;

    bool SquareGrid::isAccessible(ModelCoordinate const & curpos, ModelCoordinate const & target)
    {
        if (curpos == target) {
            return true;
        }
        int32_t const x = std::abs(target.x - curpos.x);
        int32_t const y = std::abs(target.y - curpos.y);
        if ((x <= 1) && (y <= 1)) {
            if (m_allow_diagonals || ((x ^ y) != 0)) {
                return true;
            }
        }

        return false;
    }

    double SquareGrid::getAdjacentCost(ModelCoordinate const & curpos, ModelCoordinate const & target)
    {
        if (curpos == target) {
            return 0.0;
        }
        if (std::abs(target.x - curpos.x) != std::abs(target.y - curpos.y)) {
            return 1.0;
        }
        return 1.4;
    }

    double SquareGrid::getHeuristicCost(ModelCoordinate const & curpos, ModelCoordinate const & target)
    {
        return static_cast<double>(std::abs(target.x - curpos.x) + std::abs(target.y - curpos.y));
    }

    std::string const & SquareGrid::getType() const
    {
        static std::string const type("square");
        return type;
    }

    std::string const & SquareGrid::getName() const
    {
        static std::string const squareGrid("Square Grid");
        return squareGrid;
    }

    ExactModelCoordinate SquareGrid::toMapCoordinates(ExactModelCoordinate const & layer_coords)
    {
        return m_matrix * layer_coords;
    }

    ExactModelCoordinate SquareGrid::toExactLayerCoordinates(ExactModelCoordinate const & map_coord)
    {
        return m_inverse_matrix * map_coord;
    }

    ModelCoordinate SquareGrid::toLayerCoordinates(ExactModelCoordinate const & map_coord)
    {
        ExactModelCoordinate const dblpt = toExactLayerCoordinates(map_coord);
        return toLayerCoordinatesFromExactLayerCoordinates(dblpt);
    }

    ModelCoordinate SquareGrid::toLayerCoordinatesFromExactLayerCoordinates(
        ExactModelCoordinate const & exact_layer_coords)
    {
        ModelCoordinate const result(
            static_cast<int32_t>(round(exact_layer_coords.x)),
            static_cast<int32_t>(round(exact_layer_coords.y)),
            static_cast<int32_t>(round(exact_layer_coords.z)));
        return result;
    }

    void SquareGrid::getVertices(std::vector<ExactModelCoordinate>& vtx, ModelCoordinate const & cell)
    {
        vtx.clear();
        auto x = static_cast<double>(cell.x);
        auto y = static_cast<double>(cell.y);
        vtx.emplace_back(x - 0.5, y - 0.5);
        vtx.emplace_back(x + 0.5, y - 0.5);
        vtx.emplace_back(x + 0.5, y + 0.5);
        vtx.emplace_back(x - 0.5, y + 0.5);
    }

    std::vector<ModelCoordinate> SquareGrid::toMultiCoordinates(
        ModelCoordinate const & position, std::vector<ModelCoordinate> const & orig, bool reverse)
    {
        std::vector<ModelCoordinate> coords;
        auto it = orig.begin();
        if (reverse) {
            for (; it != orig.end(); ++it) {
                ModelCoordinate mc = position;
                mc.x -= (*it).x;
                mc.y -= (*it).y;
                coords.push_back(mc);
            }
        } else {
            for (; it != orig.end(); ++it) {
                ModelCoordinate mc = position;
                mc.x += (*it).x;
                mc.y += (*it).y;
                coords.push_back(mc);
            }
        }
        return coords;
    }

    std::vector<ModelCoordinate> SquareGrid::getCoordinatesInLine(
        ModelCoordinate const & start, ModelCoordinate const & end)
    {
        std::vector<ModelCoordinate> coords;
        int32_t const dx = std::abs(end.x - start.x);
        int32_t const dy = std::abs(end.y - start.y);
        int8_t const sx  = (start.x < end.x) ? 1 : -1;
        int8_t const sy  = (start.y < end.y) ? 1 : -1;

        int32_t err  = dx - dy;
        int32_t err2 = err * 2;
        ModelCoordinate current(start);

        // We know how many points to generate: max(dx, dy) + 1
        int32_t const steps = std::max(dx, dy);

        for (int32_t i = 0; i <= steps; ++i) {
            coords.push_back(current);

            // No need to calculate the next step after the last point
            if (i == steps) {
                break;
            }

            if (err2 > -dy) {
                err -= dy;
                current.x += sx;
            } else if (err2 < dx) {
                err += dx;
                current.y += sy;
            }
            err2 = err * 2;
        }
        return coords;
    }
} // namespace FIFE
