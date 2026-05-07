// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_LOCATION_H
#define FIFE_LOCATION_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <iostream>

// 3rd party library includes

// FIFE includes
#include "model/metamodel/modelcoords.h"
#include "util/base/exception.h"

namespace FIFE
{
    class Map;
    class Layer;

    class FIFE_API Location
    {
        public:
            /** Default constructor
             */
            Location();

            /** Copy constructor
             */
            Location(Location const & loc);

            /** Utility constructor
             */
            explicit Location(Layer* layer);

            /** Destructor
             */
            ~Location() = default;

            /** Resets location (so that layer and coordinate information becomes invalid)
             */
            void reset();

            /** Assignment operator
             */
            Location& operator=(Location const & rhs);

            /** Compares equality of two locations
             */
            bool operator==(Location const & loc) const
            {
                return ((m_layer == loc.m_layer) && (m_exact_layer_coords == loc.m_exact_layer_coords));
            }

            /** Compares unequality of two locations
             */
            bool operator!=(Location const & loc) const
            {
                return !(*this == loc);
            }

            /** Gets the map where this location is pointing to
             * @note this information is fetched from the set layer
             * @return map where this location is pointing to, NULL in case its invalid
             */
            Map* getMap() const;

            /** Sets layer where this location is pointing to
             *  @param layer layer to set
             */
            void setLayer(Layer* layer);

            /** Gets the layer where this location is pointing to
             * @return layer where this location is pointing to, NULL in case its invalid
             */
            Layer* getLayer() const;

            /** Sets precise layer coordinates to this location
             *  @throws NotSet in the following cases:
             *     - layer is not set (nullptr)
             *     - layer does not have cellgrid assigned
             *  @param coordinates coordinates to set
             */
            void setExactLayerCoordinates(ExactModelCoordinate const & coordinates);

            /** Sets "cell precise" layer coordinates to this location
             *  @throws NotSet in the following cases:
             *     - layer is not set (nullptr)
             *     - layer does not have cellgrid assigned
             * @see setLayerCoordinates(const ExactModelCoordinate& coordinates)
             */
            void setLayerCoordinates(ModelCoordinate const & coordinates);

            /** Sets map coordinates to this location
             *  @param coordinates coordinates to set
             */
            void setMapCoordinates(ExactModelCoordinate const & coordinates);

            /** Gets reference to exact layer coordinates. This means that if you
             *  modify the coordinates, location gets modified directly
             * @return reference to exact layer coordinates
             */
            ExactModelCoordinate& getExactLayerCoordinatesRef();

            /** Gets exact layer coordinates set to this location
             * @return exact layer coordinates
             */
            ExactModelCoordinate getExactLayerCoordinates() const;

            /** Gets exact layer coordinates of this location mapped on given layer
             *  @throws NotSet in the following cases:
             *     - given layer is not set (nullptr)
             *     - given layer does not have cellgrid assigned
             * @return exact layer coordinates
             */
            ExactModelCoordinate getExactLayerCoordinates(Layer const * layer) const;

            /** Gets cell precision layer coordinates set to this location
             * @see getExactLayerCoordinates()
             */
            ModelCoordinate getLayerCoordinates() const;

            /** Gets cell precision layer coordinates of this location mapped on given layer
             * @see getExactLayerCoordinates(const Layer* layer)
             */
            ModelCoordinate getLayerCoordinates(Layer const * layer) const;

            /** Gets map coordinates set to this location
             * @return map coordinates
             */
            ExactModelCoordinate getMapCoordinates() const;

            /** Gets offset distance from cell center
             * @return offset distance
             */
            double getCellOffsetDistance() const;

            /** Tells if location is valid
             * Location is valid if:
             *   - layer is set
             *   - layer has cellgrid
             */
            bool isValid() const;

            /** Gets distance in map coordinates to another location on the Map
             * @param location is the location you want to get the distance to
             */
            double getMapDistanceTo(Location const & location) const;

            /** Gets layer distance to another location
             * @param location is the location you want to get the distance to
             * In case location resides on different layer, it is mapped to this layer
             */
            double getLayerDistanceTo(Location const & location) const;

        private:
            bool isValid(Layer const * layer) const;

            Layer* m_layer;
            ExactModelCoordinate m_exact_layer_coords;
    };

    /** Stream output operator.
     *
     * Useful for debugging purposes
     */
    FIFE_API std::ostream& operator<<(std::ostream&, Location const &);
} // namespace FIFE
#endif // FIFE_LOCATION_H
