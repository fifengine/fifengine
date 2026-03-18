// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

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
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "util/math/fife_math.h"

#include "squaregrid.h"

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

    bool SquareGrid::isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target)
    {
        if (curpos == target) {
            return true;
        }
        uint8_t x = std::abs(target.x - curpos.x);
        uint8_t y = std::abs(target.y - curpos.y);
        if ((x <= 1) && (y <= 1)) {
            if (m_allow_diagonals) {
                return true;
            } else if ((x ^ y) != 0) {
                return true;
            }
        }

        return false;
    }

    double SquareGrid::getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target)
    {
        if (curpos == target) {
            return 0.0;
        } else if (std::abs(target.x - curpos.x) != std::abs(target.y - curpos.y)) {
            return 1.0;
        }
        return 1.4;
    }

    double SquareGrid::getHeuristicCost(const ModelCoordinate& curpos, const ModelCoordinate& target)
    {
        return static_cast<double>(std::abs(target.x - curpos.x) + std::abs(target.y - curpos.y));
    }

    const std::string& SquareGrid::getType() const
    {
        static std::string type("square");
        return type;
    }

    const std::string& SquareGrid::getName() const
    {
        static std::string squareGrid("Square Grid");
        return squareGrid;
    }

    ExactModelCoordinate SquareGrid::toMapCoordinates(const ExactModelCoordinate& layer_coords)
    {
        return m_matrix * layer_coords;
    }

    ExactModelCoordinate SquareGrid::toExactLayerCoordinates(const ExactModelCoordinate& map_coord)
    {
        return m_inverse_matrix * map_coord;
    }

    ModelCoordinate SquareGrid::toLayerCoordinates(const ExactModelCoordinate& map_coord)
    {
        ExactModelCoordinate dblpt = toExactLayerCoordinates(map_coord);
        return toLayerCoordinatesFromExactLayerCoordinates(dblpt);
    }

    ModelCoordinate SquareGrid::toLayerCoordinatesFromExactLayerCoordinates(
        const ExactModelCoordinate& exact_layer_coords)
    {
        ModelCoordinate result(round(exact_layer_coords.x), round(exact_layer_coords.y), round(exact_layer_coords.z));
        return result;
    }

    void SquareGrid::getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell)
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
        const ModelCoordinate& position, const std::vector<ModelCoordinate>& orig, bool reverse)
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
        const ModelCoordinate& start, const ModelCoordinate& end)
    {
        std::vector<ModelCoordinate> coords;
        int32_t dx = std::abs(end.x - start.x);
        int32_t dy = std::abs(end.y - start.y);
        int8_t sx  = (start.x < end.x) ? 1 : -1;
        int8_t sy  = (start.y < end.y) ? 1 : -1;

        int32_t err  = dx - dy;
        int32_t err2 = err * 2;
        ModelCoordinate current(start);

        // We know how many points to generate: max(dx, dy) + 1
        int32_t steps = std::max(dx, dy);

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
