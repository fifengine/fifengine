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

#ifndef FIFE_MODEL_GRIDS_CELLGRID_H
#define FIFE_MODEL_GRIDS_CELLGRID_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/modelcoords.h"
#include "util/math/matrix.h"
#include "util/base/fifeclass.h"

namespace FIFE {
	class CellGrid: public FifeClass {
	public:
		/** Constructor
		 *  @param allow_diagonals if true, cellgrid allows diagonal access
		 */
 		CellGrid(bool allow_diagonals=false);

		/** Destructor
		 */
		virtual ~CellGrid();

		/** Gets the coordinates that are accesible from given point
		 *  only cells adjacent to given cell are considered in the evaluation
		 *  @param curpos position (coordinates) to evaluate
		 *  @param coordinate accessible coordinates
		 */
		void getAccessibleCoordinates(const ModelCoordinate& curpos, std::vector<ModelCoordinate>& coordinates);

		/** Type of cellgrid
		 */
		virtual const std::string& getType() const = 0;

		/** Name of the cellgrid (DEPRECATED? -jwt)
		 */
		virtual const std::string& getName() const = 0;

		/** Tells if given target point is accessible from curpos
		 *  only cells adjacent to curpos are considered in the evaluation
		 *  @param curpos position (coordinates) to evaluate
		 *  @param target target coordinate to check
		 *  @return true, if target is accessible from curpos, false otherwise
		 */
		virtual bool isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target) = 0;

		/** Returns *distance* const from curpos to target point
		 *  only cells adjacent to curpos are considered in the evaluation
		 *  @param curpos position (coordinates) to evaluate
		 *  @param target target coordinate to check
		 *  @return distance cost from curpos to target
		 */
		virtual float getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target) = 0;

		/** Gets the count of sides for a single cell
		 *  @return count of sides for a single cell
		 */
		virtual unsigned int getCellSideCount() const = 0;

		/** Transforms given point from layer coordinates to map coordinates
		 *  @return point in map coordinates
		 */
		ExactModelCoordinate toMapCoordinates(const ModelCoordinate& layer_coords);

		/** Transforms given point from layer coordinates to map coordinates
		 *  @return point in map coordinates
		 */
		virtual ExactModelCoordinate toMapCoordinates(const ExactModelCoordinate& layer_coords) = 0;
		
		/** Transforms given point from map coordinates to layer coordinates
		 *  @return point in layer coordinates
		 */
		virtual ModelCoordinate toLayerCoordinates(const ExactModelCoordinate& map_coord) = 0;

		/** Transforms given point from map coordinates to layer coordinates
		 *  @return point in layer coordinates
		 */
		virtual ExactModelCoordinate toExactLayerCoordinates(const ExactModelCoordinate& map_coord) = 0;

		/** Fills given point vector with vertices from selected cell
		 *  @param vtx vertices for given cell
		 *  @param cell cell to get vertices from
		 */
		virtual void getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell) = 0;

		/** Set the cellgrid x shift 
		 *  @param shift The shift in map coords
		 */
		void setXShift(const double& xshift) { 
			m_xshift = xshift; 
			updateMatrices();
		}

		/** Get the cellgrid x shift 
		 *  @return The x shift
		 */
		const double getXShift() const { return m_xshift; }

		/** Set the cellgrid y shift 
		 *  @param shift The shift in map coords
		 */
		void setYShift(const double yshift) {
			m_yshift = yshift; 
			updateMatrices();
		}

		/** Get the cellgrid x shift 
		 *  @return The x shift in map coords
		 */
		const double getYShift() const { return m_yshift; }

		/** Set the cellgrid x-scaling
		 *  @param scale The x-scale of cellgrid
		 */
		void setXScale(const double scale) { 
			m_xscale = scale;
			updateMatrices();
		}

		/** Set the cellgrid y-scaling
		 *  @param scale The y-scale of cellgrid
		 */
		void setYScale(const double scale) { 
			m_yscale = scale;
			updateMatrices();
		}
		
		/** Get the cellgrid x-scaling
		 *  @return The x-scale of cellgrid
		 */
		const double getXScale() const { return m_xscale; }

		/** Get the cellgrid y-scaling
		 *  @return The y-scale of cellgrid
		 */
		const double getYScale() const { return m_yscale; }
		
		/** Set the cellgrid rotation
		 *  @param rotation The rotation of the cellgrid
		 */
		void setRotation(const double rotation) { 
			m_rotation = rotation; 
			updateMatrices();
		}

		/** Get the cellgrid rotation
		 *  @return rotation The rotation of the cellgrid
		 */
		const double getRotation() const { return m_rotation; }

		/** Returns clone of this cellgrid
		 */
		virtual CellGrid* clone() = 0;

	protected:
		void updateMatrices();
		bool ptInTriangle(const ExactModelCoordinate& pt, const ExactModelCoordinate& pt1, const ExactModelCoordinate& pt2, const ExactModelCoordinate& pt3);

		DoubleMatrix m_matrix;
		DoubleMatrix m_inverse_matrix;
		double m_xshift;
		double m_yshift;
		double m_xscale;
		double m_yscale;
		double m_rotation;
		bool m_allow_diagonals;
		
	private:
		int orientation(const ExactModelCoordinate& pt, const ExactModelCoordinate& pt1, const ExactModelCoordinate& pt2);	
	};
}

#endif
