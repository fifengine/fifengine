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

#ifndef FIFE_MODEL_GRIDS_CELLGRID_H
#define FIFE_MODEL_GRIDS_CELLGRID_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/point.h"

namespace FIFE {
	class CellGrid {
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
		 *  @param coordinate accessible coordinates
		 */
		void getAccessibleCoordinates(const Point& curpos, std::vector<Point>& coordinates);

		/** Name of the cellgrid
		 */
		virtual const std::string& getName() const = 0;

		/** Tells if given target point is accessible from curpos
		 *  only cells adjacent to curpos are considered in the evaluation
		 *  @param curpos position (coordinates) to evaluate
		 *  @param target target coordinate to check
		 *  @return true, if target is accessible from curpos, false otherwise
		 */
		virtual bool isAccessible(const Point& curpos, const Point& target) = 0;

		/** Returns *distance* const from curpos to target point
		 *  only cells adjacent to curpos are considered in the evaluation
		 *  @param curpos position (coordinates) to evaluate
		 *  @param target target coordinate to check
		 *  @return distance cost from curpos to target
		 */
		virtual float getAdjacentCost(const Point& curpos, const Point& target) = 0;

		/** Return the angle between to points on the grid
		 *  @param curpos position (coordinates) to evaluate
		 *  @param target target coordinate to check
		 *  @return angle between curpos and target
		 */
		virtual float getAngleBetween(const Point& curpos, const Point& target) = 0;

		/** Set the cellgrid x shift 
		 *  @param shift The shift in elevation coords
		 */
		void setXShift(const double& xshift) { m_xshift = xshift; }

		/** Get the cellgrid x shift 
		 *  @return The x shift
		 */
		const double getXShift() const { return m_xshift; }

		/** Set the cellgrid y shift 
		 *  @param shift The shift in elevation coords
		 */
		void setYShift(const double yshift) { m_yshift = yshift; }

		/** Get the cellgrid x shift 
		 *  @return The x shift in elevation coords
		 */
		const double getYShift() const { return m_yshift; }

		/** Set the cellgrid scaling
		 *  @param scale The scale of cellgrid
		 */
		void setScale(const double scale) { m_scale = scale; }

		/** Get the cellgrid scaling
		 *  @return The scale of cellgrid
		 */
		const double getScale() const { return m_scale; }

		/** Set the cellgrid rotation
		 *  @param rotation The rotation of the cellgrid
		 */
		void setRotation(const double rotation) { m_rotation = rotation; }

		/** Get the cellgrid rotation
		 *  @return rotation The rotation of the cellgrid
		 */
		const double getRotation() const { return m_rotation; }

	protected:
		double m_xshift;
		double m_yshift;
		double m_scale;
		double m_rotation;
	};
}

#endif
