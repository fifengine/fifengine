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
#include <boost/weak_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "script/scriptcontainer.h"
#include "util/attributedclass.h"

#include "location.h"

namespace FIFE { namespace map {

	class Elevation;
	typedef boost::shared_ptr<Elevation> ElevationPtr;

	class Map;
	typedef boost::shared_ptr<Map> MapPtr;
	typedef boost::weak_ptr<Map> MapWeakPtr;

	/** A container of \c Elevation(s).
	 *
	 * The actual data is contained in \c Elevation objects; only one
	 * \c Elevation can be displayed at any time.
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

			/** Construct a map
			 */
			static MapPtr create();

			/** Destructor
			 */
			~Map();

			/** Set the maps 'name'
			 */
			void setMapName(const std::string& name);

			/** Get the maps 'name'
			 */
			const std::string& getMapName() const;

			/** Return the number of elevations on this map
			 */
			size_t getNumElevations() const;

			/** Return the elevation with a given index
			 */
			ElevationPtr getElevation(size_t index) const;

			/** Add an elevation to this map
			 */
			void addElevation(ElevationPtr);

			/** Add an elevation to this map
			 */
			void insertElevation(size_t index, ElevationPtr);

			/** Remove an elevation from this map
			 */
			void removeElevation(size_t index);

			/** Remove all elevations from a map
			 */
			void clearElevations();

			/** Apply a visitor to each elevation
			 */
			template<typename T>
			void forEachElevation(T visitor) {
				std::for_each(m_elevations.begin(),m_elevations.end(),visitor);
			}

			void setScript(ScriptType scripttype, const ScriptContainer&);
			const ScriptContainer& getScript(ScriptType scripttype);
			bool hasScript(ScriptType scripttype) const;

			bool isValidLocation(const Location& location) const;

			/** Get total number of maps
			 */
			static long globalCount();
		private:
			/** Construct a map
			 */
			Map();

			std::string m_mapname;
			MapWeakPtr m_self;
			static long m_count;

			typedef std::vector<ElevationPtr> type_elevations;
			type_elevations m_elevations;

			typedef std::map<ScriptType, ScriptContainer> type_scriptmap;
			type_scriptmap m_scripts;

			Map(const Map& map);
			Map& operator=(const Map& map);
	};

} } //FIFE::map

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
