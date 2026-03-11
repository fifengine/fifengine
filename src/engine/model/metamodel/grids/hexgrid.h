// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MODEL_GRIDS_HEXGRID_H
#define FIFE_MODEL_GRIDS_HEXGRID_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"

#include "cellgrid.h"

namespace FIFE
{
    class HexGrid : public CellGrid
    {
    public:
        explicit HexGrid(bool axial = false);
        virtual ~HexGrid();

        bool isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target);
        const std::string& getType() const;
        const std::string& getName() const;
        double getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target);
        double getHeuristicCost(const ModelCoordinate& curpos, const ModelCoordinate& target);
        uint32_t getCellSideCount() const
        {
            return 6;
        }
        ExactModelCoordinate toMapCoordinates(const ExactModelCoordinate& layer_coords);
        ModelCoordinate toLayerCoordinates(const ExactModelCoordinate& map_coord);
        ExactModelCoordinate toExactLayerCoordinates(const ExactModelCoordinate& map_coord);
        ModelCoordinate toLayerCoordinatesFromExactLayerCoordinates(const ExactModelCoordinate& exact_layer_coords);
        void getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell);
        std::vector<ModelCoordinate> toMultiCoordinates(
            const ModelCoordinate& position, const std::vector<ModelCoordinate>& orig, bool reverse);
        std::vector<ModelCoordinate> getCoordinatesInLine(const ModelCoordinate& start, const ModelCoordinate& end);
        CellGrid* clone();

    private:
        double getXZigzagOffset(double y);
        ModelCoordinate toLayerCoordinatesHelper(const ExactModelCoordinate& coords);

        bool m_axial;
    };
} // namespace FIFE

#endif
