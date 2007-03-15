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

#ifndef FIFE_MAP_LOADERS_XML_XML_H
#define FIFE_MAP_LOADERS_XML_XML_H

// Standard C++ library includes

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/loader.h"
#include "tinyxml/tinyxml.h"
#include "vfs/vfs.h"

namespace FIFE { namespace map { 
	class RawData;
	class Elevation;
	typedef boost::shared_ptr<Elevation> ElevationPtr;
	class Map;
	typedef boost::shared_ptr<Map> MapPtr;

namespace loaders { namespace xml {

	/** An XML-based maploader for the fife-engine.
	 *
	 * TinyXML: http://www.grinninglizard.com/tinyxml/
	 *
	 * DTD: https://mirror1.cvsdude.com/trac/fife/content/browser/HEAD/maps/map.dtd
	 * 
	 * @see map::Factory, XMLArchetype, ObjectLoader
	 *
	 * Notes on the map-format: http://wiki.fifengine.de/index.php?title=Map_Format
	 *
	 * Mainly inspired from FO2.
	 * 
	 * @see map.dtd The document type definition for xml maps.
	 */
	class XML : public Loader {
		public:
			/** Constructor
			 */
			XML() : Loader("XMLMap") {}

			/** Destructor
			 */
			virtual ~XML();

			/** Load a XML map
			 * @param path The XML map file ( in vfs)
			 * @return The loaded map or 0 on error.
			 */
			virtual MapPtr loadFile(const std::string& path);
			
		private:
			/// The currently loaded map.
			MapPtr m_map;

			/// Structure holding information about where the current loading takes place.
			struct s_loadcursor {
				/// Current elevation, 0 if not defined yet
				ElevationPtr elevation;
				/// starting with 0
				int elevationNumber;
				/// Current layer
				int layer;
				/// Width of the loaded map
				int width;
				/// Height of the loaded map
				int height;
			};
			s_loadcursor m_cursor;

			/// Reset internal structure.
			void cleanup();

			/// Load the whole map ...
			void loadMap(TiXmlElement*);
			/// Load an object
			void loadObject(TiXmlElement*);


			/// Load one Elevation
			void loadElevation(TiXmlElement*);

			/// Load one Layer (the real map data)
			/// dispatches to loadLayerXXX
			void loadLayer(TiXmlElement*);

#if 0
			/// Load one Layer form an image
			void loadLayerImage(TiXmlElement*);
#endif

			/// Load one Layer from a set of tile tags
			void loadLayerData(TiXmlElement*);
			/// Load one element from a set of tile tags with xy info
			void loadLayerSparseData(TiXmlElement*);

			/// Assure required Archetypes are loaded
			void loadArchetypes(TiXmlElement*);

			// Not copyable.
			XML(const XML&);
			XML& operator=(const XML&);
	};
} } } }

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
