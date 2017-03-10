/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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
#include "util/base/exception.h"
#include "util/time/timemanager.h"

#include "timeprovider.h"

namespace FIFE {
	TimeProvider::TimeProvider(TimeProvider* master): 
		m_master(master), 
		m_multiplier(1.0) {
		m_time_static = m_time_scaled = master ? master->getGameTime() : TimeManager::instance()->getTime();
	}
	
	TimeProvider::~TimeProvider() {}
	
	void TimeProvider::setMultiplier(float multiplier) { 
		if (multiplier < 0.0) {
			throw NotSupported("Negative time multiplier are not supported");
		}
		m_time_static = getPreciseGameTime();
		m_time_scaled = m_master ? m_master->getPreciseGameTime() : static_cast<float>(TimeManager::instance()->getTime());
		m_multiplier = multiplier;
	}
	
	float TimeProvider::getMultiplier() const { 
		return m_multiplier;
	}
	
	float TimeProvider::getTotalMultiplier() const {
		if (m_master) {
			return m_master->getTotalMultiplier() * m_multiplier;
		} else {
			return m_multiplier;
		}
	}
	
	uint32_t TimeProvider::getGameTime() const {
		return static_cast<uint32_t>(getPreciseGameTime());
	}
	
	double TimeProvider::getPreciseGameTime() const {
		return m_time_static + m_multiplier * ((m_master ? m_master->getPreciseGameTime() : static_cast<float>(TimeManager::instance()->getTime())) - m_time_scaled);
	}
	
	uint32_t scaleTime(float multiplier, uint32_t ticks) {
		return static_cast<uint32_t>(static_cast<float>(ticks) * multiplier);
	}
}
