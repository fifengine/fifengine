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

#ifndef FIFE_MAPTILEGRID_H
#define FIFE_MAPTILEGRID_H

#include "mapgrid.h"
#include "mapstructure.h"
#include <vector>

namespace FIFE {

	class MapTileGrid : public MapGrid {
		public:
			MapTileGrid(const Point& size, size_t geometry);
			MapTileGrid(const s_grid_info& structure);
			virtual ~MapTileGrid();

			size_t getImageId(const Point& ) const;
			void setImageId(const Point&, size_t);

			uint8_t getGlobalAlpha() const;
			void setGlobalAlpha(uint8_t);
			void addToGlobalAlpha(int delta);

		private:
			uint8_t m_global_alpha;
			std::vector<size_t> m_tiles;
	};
}

#endif
