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

#ifndef FIFE_MAP_H
#define FIFE_MAP_H

#include <string>
#include <vector>
#include <map>

#include "scriptcontainer.h"

namespace FIFE {

	class MapElevation;
	/** A container of \c MapElevation(s).
	 *
	 * The actual data is contained in \c MapElevation objects; only one
	 * \c MapElevation can be displayed at any time.
	 *
	 * Fallout legacy: A map (file or this object) contains a number of
	 * levels in one file (to switch between some level w/o loading).
	 * This has nothing to do with height-levels.
	 *
	 * Anyway, this class doesn't do anything important...
	 *
	 * @see MapElevation
	 * @see MapView
	 * @see MapLoader
	 */
	class Map {
		public:
			typedef enum {
				OnMapStartScript = 1,
				OnMapStopScript = 2,
				OnElevationChange = 3
			} ScriptType;

			Map(const std::string& mapname);
			~Map();

			const std::string& getMapName() const;
			size_t getElevationCount() const;
			MapElevation* getElevation(size_t index) const;
			void addElevation(MapElevation*);

			void setScript(ScriptType scripttype, const ScriptContainer& );
			const ScriptContainer& getScript(ScriptType scripttype);
			bool hasScript(ScriptType scripttype ) const;

		private:
			std::string m_mapname;

			typedef std::vector<MapElevation*> type_elevations;
			type_elevations m_elevations;

			typedef std::map<ScriptType,ScriptContainer> type_scriptmap;
			type_scriptmap m_scripts;

			void cleanup();
	};

}//FIFE

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
