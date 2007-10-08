/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
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
			return m_upperX - m_lowerX;
		}

		int getHeight() const {
			return m_upperY - m_lowerY;
		}

		Layer* getLayer() const {
			return m_layer;
		}
		/**
		 *
		 */
		bool isInSearchSpace(const Location& location) const;

		/**
		 *
		 */
		ModelCoordinate translateCoordsToSearchSpace(const ModelCoordinate& coords) const;

		/**
		 *
		 */
		int convertCoordToInt(const ModelCoordinate& coord) const;

		/**
		 *
		 */
		ModelCoordinate convertIntToCoord(const int cell) const;

		/**
		 *
		 */
		int getMaxIndex() const;
	private:
		//The boundries of the search space.
		int m_upperX;
		int m_upperY;
		int m_lowerX;
		int m_lowerY;

		Layer* m_layer;
	};

}


#endif
