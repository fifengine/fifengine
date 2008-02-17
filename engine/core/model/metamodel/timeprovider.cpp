/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE Team                              *
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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/exception.h"
#include "util/time/timemanager.h"

#include "timeprovider.h"

namespace FIFE {
	TimeProvider::TimeProvider(TimeProvider* master): 
		m_master(master), 
		m_multiplier(1.0) { 
	}
	
	TimeProvider::~TimeProvider() {}
	
	void TimeProvider::setMultiplier(float multiplier) { 
		if (multiplier < 0.0) {
			throw NotSupported("Negative time multiplier are not supported");
		}
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
	
	unsigned int TimeProvider::getGameTicks() const {
		if (m_master) {
			return m_master->getGameTicks();
		} else {
			return TimeManager::instance()->getTime();
		}
	}

	unsigned int scaleTime(float multiplier, unsigned int ticks) {
		return static_cast<unsigned int>(static_cast<float>(ticks) * multiplier);
	}
}
