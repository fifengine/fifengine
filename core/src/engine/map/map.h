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

#ifndef FIFE_MAP_MAP_H
#define FIFE_MAP_MAP_H

// Standard C++ library includes
#include <map>
#include <string>
#include <vector>

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "script/scriptcontainer.h"
#include "attributedclass.h"

#include "location.h"

namespace FIFE { namespace map {

	class Elevation;
	typedef boost::shared_ptr<Elevation> ElevationPtr;

	class ObjectManager;
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
	class Map : public AttributedClass {
		public:
			typedef enum {
				OnStartScript     = 1,
				OnStopScript      = 2,
				OnElevationChange = 3
			} ScriptType;

			Map(const std::string& mapname = "");
			~Map();

			void setMapName(const std::string& name);
			const std::string& getMapName() const;
			size_t getElevationCount() const;
			ElevationPtr getElevation(size_t index) const;
			void addElevation(ElevationPtr);

			template<typename T>
			void forEachElevation(T visitor) {
				std::for_each(m_elevations.begin(),m_elevations.end(),visitor);
			}

			void setScript(ScriptType scripttype, const ScriptContainer&);
			const ScriptContainer& getScript(ScriptType scripttype);
			bool hasScript(ScriptType scripttype) const;

			bool isValidLocation(const Location& location) const;
		private:
			std::string m_mapname;

			typedef std::vector<ElevationPtr> type_elevations;
			type_elevations m_elevations;

			typedef std::map<ScriptType, ScriptContainer> type_scriptmap;
			type_scriptmap m_scripts;

			void cleanup();
	};

} } //FIFE::map

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
