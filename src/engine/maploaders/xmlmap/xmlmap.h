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

#include "tinyxml/tinyxml.h"
#include "maploader.h"
#include "vfs.h"
#include "imagetool.h"
#include "mapstructure.h"
#include <list>

namespace FIFE {
	class RawData;
	class MapElevation;
	class Map;

	/** XMLMap - a xml-based maploader for the fife-engine.
	 *
	 * This is an experiment to determine what kind of features
	 * a map (and other object) format require. Furthermore it
	 * is a second test-case for the fife engine, a demo howto
	 * write a maploader and quick way to create some (free)
	 * content (that can be released with the engine).
	 *
	 * TinyXML: http://www.grinninglizard.com/tinyxml/
	 *
	 * DTD: https://mirror1.cvsdude.com/trac/fife/content/browser/HEAD/maps/map.dtd
	 * 
	 * @see MapLoader
	 * @see ImgcacheLoader
	 *
	 * Notes on the map-format:
	 *
	 * Mainly inspired from FO2 with the exception that an \em elevation
	 * (in xmlmap) only contains the data for either floor *or* roof.
	 * The first defined elevation will set the floor, the second the roof.
	 * A single xml file can only contain one (complete) map.
	 *
	 * [someone else wrote]
	 *
	 * Class for loading XML maps
	 * @warning There are a lot different  methods of actually loading the tile data,
	 * some of them may or may not be tested currently.
	 * @see map.dtd The document type definition for xml maps.
	 */
	class XMLMap : public MapLoader {
		public:
			/** Constructor
			 */
			XMLMap();

			/** Destructor
			 */
			virtual ~XMLMap();

			/** Load a XML map
			 * @param path The XML map file ( in vfs)
			 * @return The loaded map or 0 on error.
			 */
			virtual Map* loadFile(const std::string& path);
		private:
			/// Fallout Elevation Format ... used until this reader gets the capabilities to read
			/// more sopisticated map 
			s_elevation_info m_foElevationFormat;
			/// The currently loaded map.
			Map* m_map;

			/// Structure holding information about where the current loading takes place.
			struct s_loadcursor {
				/// Current elevation, 0 if not defined yet
				MapElevation* elevation;
				/// Current layer, -1 not defined, 0 floor, 1 roof -- could mean more later on
				int layer;
				/// Width of the loaded map
				int width;
				/// Height of the loaded map
				int height;
			};
			s_loadcursor m_cursor;

			/// Load next layer (aka. floor -> roof )
			void advanceElevationCursor();

			/// The ImageLoader tool
			ImgcacheLoader icL;

			XMLMap(const XMLMap&);
			XMLMap& operator=(const XMLMap&);

			/// Reset internal structure.
			void cleanup();

			/// Load tileset element
			void loadTiles(TiXmlElement*);
			/// Load spriteset element
			void loadSpriteSet(TiXmlElement*);

			/// Load the whole map ...
			void loadMap(TiXmlElement*);
			/// Load an object
			void loadObject(TiXmlElement*);

			/// Load one elevation (the real map data) -- dispatches to loadElevationXXX
			void loadElevation(TiXmlElement*);

			/// Load one elevation form an image
			void loadElevationImage(TiXmlElement*);
			/// Load one elevation from a set of tile tags
			void loadElevationData(TiXmlElement*);
			/// Load one element from a set of tile tags with xy info
			void loadElevationSparseData(TiXmlElement*);
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
