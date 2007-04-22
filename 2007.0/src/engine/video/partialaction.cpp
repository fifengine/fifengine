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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "partialaction.h"

namespace FIFE {

	PartialAction::PartialAction(uint8_t action, uint8_t  number, uint8_t direction)
		: m_warpFactor(0),
		m_action(action),
		m_number(number),
		m_direction(direction) {
	}

	PartialAction::~PartialAction() {
	}

	void PartialAction::setFirstFrame(uint8_t firstFrame) {
		m_firstFrame = firstFrame;
	}

	void PartialAction::setNumFrames(int8_t lastFrame) {
		m_numFrames = lastFrame;
	}

	void PartialAction::addNextPartialAction(const PartialActionPtr& partialAction) {
		m_partialActions.push_back( partialAction );
	}

	const PartialActionPtr& PartialAction::findNextPartialAction(
		uint8_t action, uint8_t direction) const {
		static PartialActionPtr zeroPtr;

		type_partialActions::const_iterator i(m_partialActions.begin());
		type_partialActions::const_iterator end(m_partialActions.end());

		for(; i != end; ++i) {
			if( (*i)->matches( action, direction ) ) {
				return *i;
			}
		}
		return zeroPtr;
	}



} //NS: FIFE
