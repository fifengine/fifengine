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

#ifndef FIFE_MAP_LOADERS_FALLOUT_FALLOUT_H
#define FIFE_MAP_LOADERS_FALLOUT_FALLOUT_H

// Standard C++ library includes
#include <map>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "loaders/loader.h"
#include "vfs/vfs.h"

namespace FIFE {
	class RawData;
}

namespace FIFE { namespace map { namespace loaders { namespace fallout {

	class Header;
	class PALAnimation;

	/** Fallout 1/2 data MapLoader implementation.
	 *
	 * \see map::Loader
	 *
	 * This class uses all other fallout-specific classes (especially
	 * \c ObjectFactory) to create the fife-defined classes from
	 * the original files.
	 * 
	 * @bug The internal caching mechanism duplicates functionality.
	 * @bug Palette-Animations are not generated. (FIXED)
	 *
	 * It ~bridges~ between the important fife classes and the fallout
	 * formats; the most important are:
	 *
	 * \see Header
	 * \see ObjectFactory
	 * \see FRM
	 *
	 * \see Object
	 * \see map::Tile
	 * \see map::Elevation
	 * \see map::Map
	 */
	class Fallout : public Loader {
		public:
			/** Constructor
			 * @note This will also add the DAT1 and DAT2 Source providers and
			 * and try to load master.dat and critter.dat into the VFS.
			 */
			Fallout();
			virtual ~Fallout();

			/** Try to load a .map format mapfile.
			 * @param path VFS path to the mapfile
			 * @return Pointer to a \c Map instance
			 *
			 * \note This function propagates exceptions outward: not found, 
			 * invalid format.
			 */
			virtual MapPtr loadFile(const std::string& path);

		private:
			Header* m_header;

			struct s_imgdata {
				size_t cacheid;
				int shift_x;
				int shift_y;
				PALAnimation* pal_animation;
			};

			typedef std::map<std::string, s_imgdata> type_cached;
			type_cached m_cached;

			/** Create Fallout style elevation
			 */
			ElevationPtr createElevation();

			/** Load Floor and Roof tiles into the Map
			 */
			void loadTiles(MapPtr map, RawDataPtr data);

			/** Skip over scripts
			 */
			void ignoreScripts(RawDataPtr data);

			/** Load Objects
			 */
			void loadObjects(MapPtr map, RawDataPtr data);

			/** Clean up internal data
			 */
			void cleanup();

			/** Get the Image data associated with a path
			 */
			const s_imgdata& getImageID(const std::string& path);

			// Not copyable
			Fallout(const Fallout&);
			Fallout& operator=(const Fallout&);
	};

} } } } //FIFE::map::loaders::fallout

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
