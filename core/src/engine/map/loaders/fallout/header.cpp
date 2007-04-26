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

// Standard C++ library includes
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "debugutils.h"
#include "util/exception.h"

#include "header.h"

// If (flag & 0x1) == 0 then ?? unknown.
// If (flag & 0x2) == 0 then the map has an elevation at level 0.
// If (flag & 0x4) == 0 then the map has an elevation at level 1.
// If (flag & 0x8) == 0 then the map has an elevation at level 2.

static const uint32_t ELEVATION_LEVEL0 = 0x02;
static const uint32_t ELEVATION_LEVEL1 = 0x04;
static const uint32_t ELEVATION_LEVEL2 = 0x08;

namespace FIFE { namespace map { namespace loaders { namespace fallout {

	Header::Header(RawDataPtr data) : 
		m_mapname(), m_version(0), m_defaultPlayerPos(0), m_defaultElevation(0),
		m_defaultOrientation(0), m_num_localvars(0), m_scriptid(0), 
		m_elevations(0), m_num_globalvars(0), m_mapID(0), m_timestamp(0) {
		loadHeader(data);
	}

	// Does this really need to be factored out and public?
	void Header::loadHeader(RawDataPtr data) {
		data->setIndex(0);

		m_version = data->read32Big();
		m_mapname = data->readString(16);
		m_defaultPlayerPos = data->read32Big();
		m_defaultElevation = data->read32Big();
		m_defaultOrientation = data->read32Big();
		m_num_localvars = data->read32Big();
		m_scriptid = data->read32Big();
		m_elevations = data->read32Big();
		data->read32Big(); // unknown
		m_num_globalvars = data->read32Big();
		m_mapID = data->read32Big();
		m_timestamp = data->read32Big();
		data->moveIndex(4*44);

		assert(data->getCurrentIndex() == 0xec);
		checkSanity();

		Log("mlfallout") << "Version: " << m_version;
		Log("mlfallout") << "Mapname: " << m_mapname;
		Log("mlfallout") << "Default Startpos: " << m_defaultPlayerPos;
		Log("mlfallout") << "Default Elevation: " << m_defaultElevation;
		Log("mlfallout") << "Default Orientation: " << m_defaultOrientation;
		Log("mlfallout") << "Localvars: " << m_num_localvars;
		Log("mlfallout") << "ScriptID: " << m_scriptid;

		std::string str_elevations;
		if (!(m_elevations & ELEVATION_LEVEL0)) {
			str_elevations += " 0";
		}
		if (!(m_elevations & ELEVATION_LEVEL1)) {
			str_elevations += " 1";
		}
		if (!(m_elevations & ELEVATION_LEVEL2)) {
			str_elevations += " 2";
		}

		Log("mlfallout") << "Elevationflags: " 
		                 /*<< std::hex*/ << m_elevations 
		                 /*<< std::dec*/ << " ->" << str_elevations;
		Log("mlfallout") << "Globalvars: " << m_num_globalvars;
		Log("mlfallout") << "MapID: " << m_mapID;
		Log("mlfallout") << "Timestamp: " << m_timestamp;
	}

	void Header::checkSanity() {
		if (m_version != 19 && m_version != 20) {
			Log("Header") << "invalid version: " << m_version;
			throw InvalidFormat("invalid version");
		}

		if (!getNumElevations()) {
			throw InvalidFormat("no elevations? :(");
		}
	}

	const std::string& Header::getMapName() const {
		return m_mapname;
	}

	unsigned int Header::getNumElevations() const {
		unsigned int numelevations = 0;
		if (!(m_elevations & ELEVATION_LEVEL0)) {
			++numelevations;
		}
		if (!(m_elevations & ELEVATION_LEVEL1)) {
			++numelevations;
		}
		if (!(m_elevations & ELEVATION_LEVEL2)) {
			++numelevations;
		}

		return numelevations;
	}

	uint32_t Header::getVersion() const {
		return m_version;
	}

	uint32_t Header::getElevations() const {
		return m_elevations;
	}

	uint32_t Header::getNumLocalVars() const {
		return m_num_localvars;
	}

	uint32_t Header::getNumGlobalVars() const {
		return m_num_globalvars;
	}

	bool Header::hasElevation(unsigned int num) const {
		if (num >= 3)
			return false;

		return !(m_elevations & (ELEVATION_LEVEL0 << num));
	}

	int32_t Header::getDefaultElevation() const {
		return m_defaultElevation;
	}

	int32_t Header::getDefaultPlayerPosition() const {
		return m_defaultPlayerPos;
	}

} } } }
