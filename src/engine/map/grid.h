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

#ifndef FIFE_MAP_GRID_H
#define FIFE_MAP_GRID_H

// Standard C++ library includes
#include <map>
#include <string>
#include <vector>

// 3rd party library includes
#include <boost/variant.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/point.h"
#include "attributedclass.h"
#include "exception.h"

namespace FIFE { namespace map {

	class Geometry;
	/** A basic layer on a map elevation
	 *
	 *  This class represents a layer on the Map.
	 *  This can be for example a Tile layer 
	 *  as the roofs and floors of a fo2 map
	 *  but can also just contain "objects"
	 *  on grid coords.
	 * 
	 *  The tiles are *not* allways created only on
	 *  a first "setTileImage".
 	 *  
	 *  The most important features of this class are
	 *  "geometry", "shift" and "size":
	 *
	 *  The geometry is used to position objects on this
	 *  Grid and the Tiles too.
	 *
	 *  The shift is added to all screen coords and
	 *  will create the illusion of a height-difference :-)
	 *
	 *  The size simply is the maximum allowd size in grid
	 *  coords this grid covers.
	 *
	 *  Attributes:
	 *  <pre>
	 * 	name:		Name of the grid ("floor")
	 *  </pre>
	 *  
	 *  Future:
	 *  	Connections between grids to walk through (Elevators...)
	 *	Grouping of grids (These grids are roofs ... etc)
	 */
	class Grid : public AttributedClass {
		public:
			typedef std::vector<long> type_paramgrid;

			/** Constructs a MapGrid instance
			 */
			Grid(const Point& size, size_t geometry);

			/** Get the Geometry as set in the constructor
			 *  @return a valid Geometry
			 */
			Geometry* getGeometry();

			/** Get the size as set in the constructor
			 *  @return the size in grid coords
			 */
			const Point& getSize() const;

			/** Set the screen coord shift
			 *  @param shift The new shift in screen coords
			 */
			void setShift(const Point& shift);

			/** Get the screen coord shift
			 *  @return The shift in screen coords
			 */
			const Point& getShift() const;

			/** Get the tile image id of a position
			 *  @note If the position is invalid or the grid
			 *	is not yet created 0 is returned (a DummyImage id)
			 *  @param position A valid grid position
			 *  @return An image id for the position
			 */
			size_t getTileImage(const Point&) const;
			size_t getTileImage(int32_t x,int32_t y) const;

			/** Get the tile image id of a position
			 *  @note If the position is invalid nothing is changed
			 *  @note If no tile have previously set, this
			 *  will allocate the needed memory.
			 *  @param position A valid grid position
			 *  @param image_id An image id for the position
			 */
			void setTileImage(const Point& position, size_t image_id);
			void setTileImage(int32_t x,int32_t y, size_t image_id);

			/** API for grid point parameters
			 *  @bug THIS WILL FAIL - WIP
			 */
			template<typename T>
			std::vector<T>& getParamGrid(uint8_t type) {
				return boost::get<T>(m_paramgrids.at(type));
			}
			
			/** API for grid point parameters
			 *  @bug THIS WILL FAIL - WIP
			 */
			template<typename T>
			const std::vector<T>& getParamGrid(uint8_t type) const {
				return boost::get<T>(m_paramgrids.at(type));
			}
			
			/** API for grid point parameters
			 *  @bug THIS WILL FAIL - WIP
			 */
			template<typename T>
			const T& getParam(const Point& p, uint8_t type) const {
				return boost::get<T>(m_paramgrids.at(type)).at(p.x + p.y * m_size.x);
			}

			/** API for grid point parameters
			 *  @bug THIS WILL FAIL - WIP
			 */
			template<typename T>
			T& getParam(const Point& p, uint8_t type)  {
				return boost::get<T>(m_paramgrids.at(type)).at(p.x + p.y * m_size.x);
			}

			/** API for grid point parameters
			 *  @bug THIS WILL FAIL - WIP
			 */
			template<typename T>
			void setParam(const Point& p, uint8_t type, const T& value) {
				boost::get<T>(m_paramgrids.at(type)).at(p.x + p.y * m_size.x) = value;
			}
			/** Set a param value
			 * @see setParam
			 */

			template<typename T>
			void setParam(int16_t x,int16_t y, uint8_t type, const T& value) {
				boost::get<T>(m_paramgrids.at(type)).at(x + y * m_size.x) = value;
			}

			/** API for grid point parameters
			 *  @bug THIS WILL FAIL - WIP
			 */
			template<typename T>
			uint8_t addParam(const std::string& name, const T& _default = T()) {

				if( m_paramnames.find(name) == m_paramnames.end() ) {
					throw NameClash(name);
				}

				if( m_paramgrids.size() == 250 ) {
					throw NotSupported("More than 250 parameters per grid.");
				}

				uint8_t param_id = m_paramgrids.size();
				m_paramgrids.push_back( std::vector<T>() );
				boost::get<std::vector<T> >(m_paramgrids[ param_id ])
					.resize(m_size.x*m_size.y, _default);
				m_paramnames[ name ] = param_id;
				return param_id;
			}

			/** API for grid point parameters
			 *  @bug THIS WILL FAIL - WIP
			 */
			uint8_t getParamByName(const std::string& name) const {
				type_paramnames::const_iterator i = m_paramnames.find(name);
				if( i == m_paramnames.end() ) {
					throw NotFound(name);
				}
				return i->second;
			}

			bool hasParamByName(const std::string& name) const {
				return m_paramnames.find(name) != m_paramnames.end();
			}

			/** Set Tiles visible
			 */
			void setTilesVisible(bool vis);

			/** Toggle tile visibility
			 * @see setTilesVisible
			 */
			void toggleTilesVisible();

			/** Check tile visibility
			 * @see setTilesVisible
			 */
			bool areTilesVisible() const;

			/** Set object visibility
			 */
			void setObjectsVisible(bool vis);

			/** Toggle object visibility
			 *  @see setObjectsVisible
			 */
			void toggleObjectsVisible();

			/** Check object visibility
			 *  @see setObjectsVisible
			 */
			bool areObjectsVisible() const;

			/** Set global tile alpha value
			 *  All tiles are rendered with this alpha value.
			 */
			void setGlobalAlpha(uint8_t);

			/** Get global tile alpha value
			 *  @see setGlobalAlpha
			 */
			uint8_t getGlobalAlpha() const;

			/** Change global tile alpha value
			 *  Ensures no overflow happens and the alpha
			 *  value stays between 0 (transparent) and
			 *  255(opaque)
			 *  @see setGlobalAlpha
			 */
			void addToGlobalAlpha(int delta);

			/** Check whether the grid lines shall be drawn
			 *  Grid lines are drawn by setting an overlay
			 *  image, which is drawn over each tiles position
			 */
			bool isGridOverlayEnabled() const;
			void setGridOverlayEnabled(bool e);
			void toggleGridOverlayEnabled();

			/** Check whether a position is on the grid
			 *  @param positon A grid position to check
			 *  @return True, only if the position is on the grid
			 */
			bool isValidPosition(const Point& pos) const;

			const std::string& getOverlayImage() const;
			void setOverlayImage(const std::string& fname);

		protected:
			Point m_size;

			uint8_t m_global_alpha;

			bool m_tiles_visibility;
			bool m_objects_visibility;
			bool m_grid_overlay;

			std::vector<size_t> m_tiles;

			Point m_shift;
			Geometry* m_geometry;
			std::string m_overlay_image;

			typedef boost::variant<
				std::vector<long>,
				std::vector<bool>,
				std::vector<float>
			> type_vargrid;

			std::vector<type_vargrid> m_paramgrids;
			typedef std::map<std::string,uint8_t> type_paramnames ;
			type_paramnames m_paramnames;
	};
} } // FIFE::map

#endif
