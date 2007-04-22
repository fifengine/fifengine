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

#ifndef IANMAPFORMATFALLOUT_H
#define IANMAPFORMATFALLOUT_H

#include "maploader.h"
#include "mapstructure.h"
#include <map>
#include <string>
#include "vfs.h"

namespace FIFE {

	class RawData;
	class MFFalloutHeader;
	class PALAnimation;

	/** Fallout 1/2 data MapLoader implementation.
	 *
	 * \see MapLoader
	 *
	 * This class uses all other fallout-specific classes (especially
	 * \c MFFalloutObjectFactory) to create the fife-defined classes from
	 * the original files.
	 * 
	 * @bug The internal caching mechanism duplicates functionality.
	 * @bug Palette-Animations are not generated. (FIXED)
	 *
	 * It ~bridges~ between the important fife classes and the fallout
	 * formats; the most important are:
	 *
	 * \see MFFalloutHeader
	 * \see MFFalloutObjectFactory
	 * \see MFFalloutFRM
	 *
	 * \see Object
	 * \see MapTile
	 * \see MapElevation
	 * \see Map
	 */
	class MFFallout : public MapLoader {
		public:
			/** Constructor
			 * @note This will also add the DAT1 and DAT2 Source providers and
			 * and try to load master.dat and critter.dat into the VFS.
			 */
			MFFallout();
			virtual ~MFFallout();

			/** Try to load a .map format mapfile.
			 * @param path VFS path to the mapfile
			 * @return Pointer to a \c Map instance
			 *
			 * \note This function propagates exceptions outward: not found, invalid format.
			 */
			virtual Map* loadFile(const std::string& path);

		private:
			MFFalloutHeader* m_header;
			s_elevation_info m_foElevationFormat;

			struct s_imgdata {
				size_t cacheid;
				int shift_x;
				int shift_y;
				PALAnimation* pal_animation;
			};

			typedef std::map<std::string, s_imgdata> type_cached;
			type_cached m_cached;

			/** Load Floor and Roof tiles into the Map
			 */
			void loadTiles(Map* map, RawDataPtr data);

			/** Skip over scripts
			 */
			void ignoreScripts(RawDataPtr data);

			/** Load Objects
			 */
			void loadObjects(Map* map, RawDataPtr data);

			/** Clean up internal data
			 */
			void cleanup();

			/** Get the Image data associated with a path
			 */
			const s_imgdata& getImageID(const std::string& path);

			MFFallout(const MFFallout&);
			MFFallout& operator=(const MFFallout&);
	};

}//FIFE

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
