/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#ifndef FIFE_MAPGRID_H
#define FIFE_MAPGRID_H

#include "point.h"
#include <string>

namespace FIFE {

	class GridGeometry;

	class MapGrid {
		public:
			MapGrid(const Point& size, size_t geometry);
			virtual ~MapGrid();

			GridGeometry* getGeometry();

			const Point& getSize() const;

			void setShift(const Point&);
			const Point& getShift() const;

			void setGridOverlay(const std::string& );
			const std::string& getGridOverlay() const;

		protected:
			Point m_size;

		private:
			std::string m_grid_overlay;
			Point m_shift;
			GridGeometry* m_geometry;
	};
}

#endif
