// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MODEL_GRIDS_CELLGRID_H
#define FIFE_MODEL_GRIDS_CELLGRID_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "model/metamodel/modelcoords.h"
#include "util/base/fife_stdint.h"
#include "util/base/fifeclass.h"
#include "util/math/matrix.h"

namespace FIFE
{
    class /*FIFE_API*/ CellGrid : public FifeClass
    {
        public:
            /** Constructor
             */
            CellGrid();

            /** Destructor
             */
            virtual ~CellGrid();

            /** Gets the coordinates that are accesible from given point
             *  only cells adjacent to given cell are considered in the evaluation
             *  @param curpos position (coordinates) to evaluate
             *  @param coordinates accessible coordinates
             */
            void getAccessibleCoordinates(ModelCoordinate const & curpos, std::vector<ModelCoordinate>& coordinates);

            /** Type of cellgrid
             */
            virtual std::string const & getType() const = 0;

            /** Name of the cellgrid (DEPRECATED? -jwt)
             */
            virtual std::string const & getName() const = 0;

            /** Tells if given target point is accessible from curpos
             *  only cells adjacent to curpos are considered in the evaluation
             *  @param curpos position (coordinates) to evaluate
             *  @param target target coordinate to check
             *  @return true, if target is accessible from curpos, false otherwise
             */
            virtual bool isAccessible(ModelCoordinate const & curpos, ModelCoordinate const & target) = 0;

            /** Returns *distance* const from curpos to target point
             *  only cells adjacent to curpos are considered in the evaluation
             *  @param curpos position (coordinates) to evaluate
             *  @param target target coordinate to check
             *  @return distance cost from curpos to target
             */
            virtual double getAdjacentCost(ModelCoordinate const & curpos, ModelCoordinate const & target) = 0;

            /** Returns *distance* const from curpos to target point
             *  @param curpos position (coordinates) to evaluate
             *  @param target target coordinate to check
             *  @return distance cost from curpos to target
             */
            virtual double getHeuristicCost(ModelCoordinate const & curpos, ModelCoordinate const & target) = 0;

            /** Gets the count of sides for a single cell
             *  @return count of sides for a single cell
             */
            virtual uint32_t getCellSideCount() const = 0;

            /** Transforms given point from layer coordinates to map coordinates
             *  @return point in map coordinates
             */
            ExactModelCoordinate toMapCoordinates(ModelCoordinate const & layer_coords);

            /** Transforms given point from layer coordinates to map coordinates
             *  @return point in map coordinates
             */
            virtual ExactModelCoordinate toMapCoordinates(ExactModelCoordinate const & layer_coords) = 0;

            /** Transforms given point from map coordinates to layer coordinates
             *  @return point in layer coordinates
             */
            virtual ModelCoordinate toLayerCoordinates(ExactModelCoordinate const & map_coord) = 0;

            /** Transforms given point from map coordinates to layer coordinates
             *  @return point in layer coordinates
             */
            virtual ExactModelCoordinate toExactLayerCoordinates(ExactModelCoordinate const & map_coord) = 0;

            /** Transforms given point from exact layer coordinates to cell precision layer coordinates
             *  @return point in layer coordinates
             */
            virtual ModelCoordinate toLayerCoordinatesFromExactLayerCoordinates(
                ExactModelCoordinate const & exact_layer_coords) = 0;

            /** Fills given point vector with vertices from selected cell
             *  @param vtx vertices for given cell
             *  @param cell cell to get vertices from
             */
            virtual void getVertices(std::vector<ExactModelCoordinate>& vtx, ModelCoordinate const & cell) = 0;

            /** Returns point vector with coordinates for a multi object
             *  @param position The center position
             *  @param orig The vector with the original coordinates from object
             *  @param reverse If true the orig coordinates are subtracted otherwise they are added
             */
            virtual std::vector<ModelCoordinate> toMultiCoordinates(
                ModelCoordinate const & position, std::vector<ModelCoordinate> const & orig, bool reverse = false) = 0;

            /** Returns point vector with coordinates for a line from start to end.
             * @param start The start position
             * @param end The end position
             * @return vector with points
             */
            virtual std::vector<ModelCoordinate> getCoordinatesInLine(
                ModelCoordinate const & start, ModelCoordinate const & end) = 0;

            /** Set the cellgrid x shift
             *  @param xshift The shift in map coords
             */
            void setXShift(double const & xshift)
            {
                m_xshift = xshift;
                updateMatrices();
            }

            /** Get the cellgrid x shift
             *  @return The x shift
             */
            double getXShift() const
            {
                return m_xshift;
            }

            /** Set the cellgrid y shift
             *  @param yshift The shift in map coords
             */
            void setYShift(double const yshift)
            {
                m_yshift = yshift;
                updateMatrices();
            }

            /** Get the cellgrid y shift
             *  @return The y shift in map coords
             */
            double getYShift() const
            {
                return m_yshift;
            }

            /** Set the cellgrid z shift
             *  @param zshift The shift in map coords
             */
            void setZShift(double const zshift)
            {
                m_zshift = zshift;
                updateMatrices();
            }

            /** Get the cellgrid z shift
             *  @return The z shift in map coords
             */
            double getZShift() const
            {
                return m_zshift;
            }

            /** Set the cellgrid x-scaling
             *  @param scale The x-scale of cellgrid
             */
            void setXScale(double const scale)
            {
                m_xscale = scale;
                updateMatrices();
            }

            /** Set the cellgrid y-scaling
             *  @param scale The y-scale of cellgrid
             */
            void setYScale(double const scale)
            {
                m_yscale = scale;
                updateMatrices();
            }

            /** Set the cellgrid z-scaling
             *  @param scale The z-scale of cellgrid
             */
            void setZScale(double const scale)
            {
                m_zscale = scale;
                updateMatrices();
            }

            /** Get the cellgrid x-scaling
             *  @return The x-scale of cellgrid
             */
            double getXScale() const
            {
                return m_xscale;
            }

            /** Get the cellgrid y-scaling
             *  @return The y-scale of cellgrid
             */
            double getYScale() const
            {
                return m_yscale;
            }

            /** Get the cellgrid z-scaling
             *  @return The z-scale of cellgrid
             */
            double getZScale() const
            {
                return m_zscale;
            }

            /** Set the cellgrid rotation
             *  @param rotation The rotation of the cellgrid
             */
            void setRotation(double const rotation)
            {
                m_rotation = rotation;
                updateMatrices();
            }

            /** Get the cellgrid rotation
             *  @return rotation The rotation of the cellgrid
             */
            double getRotation() const
            {
                return m_rotation;
            }

            /** Set whether diagonal cell access is allowed
             *  @param allow_diagonals True if diagonal access allowed, false if only cell edges allowed
             */
            void setAllowDiagonals(bool const allow_diagonals)
            {
                m_allow_diagonals = allow_diagonals;
            }

            /** Get whether diagonal cell access is allowed
             *  @return True if diagonal access allowed, false if only cell edges allowed
             */
            bool getAllowDiagonals() const
            {
                return m_allow_diagonals;
            }

            /** Returns clone of this cellgrid
             */
            virtual CellGrid* clone() = 0;

        protected:
            void updateMatrices();
            bool ptInTriangle(
                ExactModelCoordinate const & pt,
                ExactModelCoordinate const & pt1,
                ExactModelCoordinate const & pt2,
                ExactModelCoordinate const & pt3);

            DoubleMatrix m_matrix;
            DoubleMatrix m_inverse_matrix;
            double m_xshift;
            double m_yshift;
            double m_zshift;
            double m_xscale;
            double m_yscale;
            double m_zscale;
            double m_rotation;
            bool m_allow_diagonals;

        private:
            int32_t orientation(
                ExactModelCoordinate const & pt, ExactModelCoordinate const & pt1, ExactModelCoordinate const & pt2);
    };
} // namespace FIFE

#endif
