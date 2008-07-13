/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_PATHFINDER_SEARCHSPACE
#define FIFE_PATHFINDER_SEARCHSPACE

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/location.h"

namespace FIFE {

	class Layer;

	class SearchSpace {
	public:
		SearchSpace(Layer* layer);

		int getUpperX() const {
			return m_upperX;
		}

		int getUpperY() const {
			return m_upperY;
		}

		int getLowerX() const {
			return m_lowerX;
		}

		int getLowerY() const {
			return m_lowerY;
		}

		int getWidth() const { 
			//1 is added to make it inclusive of the first cell.
			return (m_upperX - m_lowerX) + 1;
		}

		int getHeight() const {
			return (m_upperY - m_lowerY) + 1;
		}

		Layer* getLayer() const {
			return m_layer;
		}

		/** Determines whether the given location is within the searchspace.
		 *
		 * Tests the equality of the layers and then tests to see whether the coordinates
		 * lie within the search space.
		 *
		 * @param location The location to test.
		 * @return True if it is in the search space, false otherwise.
		 */
		bool isInSearchSpace(const Location& location) const;

		/** Translates coordinates into the search space.
		 *
		 * given a coordinate this function returns that coordinate in relation to the
		 * search space origin.
		 *
		 * @param coords The coordinate to translate.
		 * @return The translated coordinate.
		 */
		ModelCoordinate translateCoordsToSearchSpace(const ModelCoordinate& coords) const;

		/** Converts a coordinate into a unique integer id.
		 *
		 * Takes a model coordinate and based on the coordinate returns a unique identifier.
		 *
		 * @param coord The model coord to get the integer of.
		 * @return The unique identifier.
		 */
		int convertCoordToInt(const ModelCoordinate& coord) const;

		/** Converts an integer to a ModelCoordinate.
		 *
		 * Converts an integer that represents a cell in the search space into the
		 * actual ModelCoordinate representing it's physical location.
		 *
		 * @param cell An integer of the cell to get the coordinate of.
		 * @return The model coordinate in question.
		 */
		ModelCoordinate convertIntToCoord(const int cell) const;

		/** Returns the maximum index on the layer.
		 * Returns the maximum indexible coordinate on the search space.
		 * @return The maximum indexible coordinate.
		 */
		int getMaxIndex() const;
	private:
		//The boundries of the search space.
		int m_upperX;
		int m_upperY;
		int m_lowerX;
		int m_lowerY;

		//The layer of the search space.
		Layer* m_layer;
	};

}


#endif
