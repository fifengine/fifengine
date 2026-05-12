// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MODEL_GRIDS_HEXGRID_H
#define FIFE_MODEL_GRIDS_HEXGRID_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "cellgrid.h"
#include "util/base/fife_stdint.h"

namespace FIFE
{
    class FIFE_API HexGrid : public CellGrid
    {
        public:
            explicit HexGrid(bool axial = false);
            ~HexGrid() override;

            bool isAccessible(ModelCoordinate const & curpos, ModelCoordinate const & target) override;
            std::string const & getType() const override;
            std::string const & getName() const override;
            double getAdjacentCost(ModelCoordinate const & curpos, ModelCoordinate const & target) override;
            double getHeuristicCost(ModelCoordinate const & curpos, ModelCoordinate const & target) override;
            uint32_t getCellSideCount() const override
            {
                return 6;
            }
            ExactModelCoordinate toMapCoordinates(ExactModelCoordinate const & layer_coords) override;
            ModelCoordinate toLayerCoordinates(ExactModelCoordinate const & map_coord) override;
            ExactModelCoordinate toExactLayerCoordinates(ExactModelCoordinate const & map_coord) override;
            ModelCoordinate toLayerCoordinatesFromExactLayerCoordinates(
                ExactModelCoordinate const & exact_layer_coords) override;
            void getVertices(std::vector<ExactModelCoordinate>& vtx, ModelCoordinate const & cell) override;
            std::vector<ModelCoordinate> toMultiCoordinates(
                ModelCoordinate const & position, std::vector<ModelCoordinate> const & orig, bool reverse) override;
            std::vector<ModelCoordinate> getCoordinatesInLine(
                ModelCoordinate const & start, ModelCoordinate const & end) override;
            CellGrid* clone() override;

        private:
            double getXZigzagOffset(double y) const;
            ModelCoordinate toLayerCoordinatesHelper(ExactModelCoordinate const & coords) const;

            bool m_axial;
    };
} // namespace FIFE

#endif
