/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/modelcoords.h"
#include "util/log/logger.h"

#include "triggercontroller.h"
#include "trigger.h"
#include "map.h"

namespace FIFE {

	static Logger _log(LM_STRUCTURES);

	TriggerController::TriggerController(Map* map) :
		m_map(map){

	}

	TriggerController::~TriggerController() {

	}

	Trigger* TriggerController::addCellTriggerToLayer(const std::string& triggerName, Layer* layer, const ModelCoordinate& pt1) {
		assert(layer);
		assert(!exists(triggerName));

		Trigger* trigger = new Trigger(triggerName);

		std::pair<TriggerNameMapIterator, bool> returnValue;
		returnValue = m_triggerNameMap.insert ( TriggerNameMapPair(triggerName, trigger));

		if (!returnValue.second) {
			FL_WARN(_log, LMsg("TriggerController::addCellTriggerToLayer() - ") << "Trigger " << triggerName << " already exists.... ignoring.");
		}

		return returnValue.first->second;
	}

	bool TriggerController::exists(const std::string& name) {
		TriggerNameMapIterator it = m_triggerNameMap.find(name);
		if (it != m_triggerNameMap.end()) {
			return true;
		}

		return false;
	}

} //FIFE
