/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_LOCATION_H
#define FIFE_LOCATION_H

// Standard C++ library includes
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/modelcoords.h"
#include "util/base/exception.h"

namespace FIFE {
	class Map;
	class Layer;

	class Location {
	public:
		/** Default constructor
		 */
		Location();
		
		/** Copy constructor
		 */
		Location(const Location& loc);
		
		/** Utility constructor
		 */
		Location(Layer* layer);
		
		/** Destructor
		 */
		~Location();
		
		/** Resets location (so that layer and coordinate information becomes invalid)
		 */
		void reset();
		
		/** Assignment operator
		 */
		Location& operator=(const Location& rhs);
		
		/** Compares equality of two locations
		 */
		inline bool operator==(const Location& loc) const {
			return ((m_layer == loc.m_layer) && (m_exact_layer_coords == loc.m_exact_layer_coords));
		}
		
		/** Compares unequality of two locations
		 */
		inline bool operator!=(const Location& loc) const {
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
		 *     - layer is not set (NULL)
		 *     - layer does not have cellgrid assigned
		 *  @param coordinates coordinates to set
		 */
		void setExactLayerCoordinates(const ExactModelCoordinate& coordinates);
		
		/** Sets "cell precise" layer coordinates to this location
		 *  @throws NotSet in the following cases:
		 *     - layer is not set (NULL)
		 *     - layer does not have cellgrid assigned
		 * @see setLayerCoordinates(const ExactModelCoordinate& coordinates)
		 */
		void setLayerCoordinates(const ModelCoordinate& coordinates);

		/** Sets map coordinates to this location
		 *  @param coordinates coordinates to set
		 */
		void setMapCoordinates(const ExactModelCoordinate& coordinates);

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
		 *     - given layer is not set (NULL)
		 *     - given layer does not have cellgrid assigned
		 * @return exact layer coordinates
		 */
		ExactModelCoordinate getExactLayerCoordinates(const Layer* layer) const;
		
		/** Gets cell precision layer coordinates set to this location
		 * @see getExactLayerCoordinates()
		 */
		ModelCoordinate getLayerCoordinates() const;
		
		/** Gets cell precision layer coordinates of this location mapped on given layer
		 * @see getExactLayerCoordinates(const Layer* layer)
		 */
		ModelCoordinate getLayerCoordinates(const Layer* layer) const;
		
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
		double getMapDistanceTo(const Location& location) const;

		/** Gets layer distance to another location
		 * @param location is the location you want to get the distance to
		 * In case location resides on different layer, it is mapped to this layer
		 */
		double getLayerDistanceTo(const Location& location) const;
		
	private:
		bool isValid(const Layer* layer) const;
		
		Layer* m_layer;
		ExactModelCoordinate m_exact_layer_coords;
	};
	
	/** Stream output operator.
	 *
	 * Useful for debugging purposes
	 */
	std::ostream& operator<<(std::ostream&, const Location&);
}
#endif //FIFE_LOCATION_H
