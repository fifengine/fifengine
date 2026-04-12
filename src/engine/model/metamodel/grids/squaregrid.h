// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MODEL_GRIDS_SQUAREGRID_H
#define FIFE_MODEL_GRIDS_SQUAREGRID_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "util/base/fife_stdint.h"

#include "cellgrid.h"

namespace FIFE
{
    class /*FIFE_API*/ SquareGrid : public CellGrid
    {
    public:
        SquareGrid();
        ~SquareGrid() override;

        const std::string& getType() const override;
        const std::string& getName() const override;
        bool isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target) override;
        double getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target) override;
        double getHeuristicCost(const ModelCoordinate& curpos, const ModelCoordinate& target) override;
        uint32_t getCellSideCount() const override
        {
            return 4;
        }
        ExactModelCoordinate toMapCoordinates(const ExactModelCoordinate& layer_coords) override;
        ModelCoordinate toLayerCoordinates(const ExactModelCoordinate& map_coord) override;
        ExactModelCoordinate toExactLayerCoordinates(const ExactModelCoordinate& map_coord) override;
        ModelCoordinate toLayerCoordinatesFromExactLayerCoordinates(
            const ExactModelCoordinate& exact_layer_coords) override;
        void getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell) override;
        std::vector<ModelCoordinate> toMultiCoordinates(
            const ModelCoordinate& position, const std::vector<ModelCoordinate>& orig, bool reverse) override;
        std::vector<ModelCoordinate> getCoordinatesInLine(
            const ModelCoordinate& start, const ModelCoordinate& end) override;
        CellGrid* clone() override;
    };
} // namespace FIFE

#endif
