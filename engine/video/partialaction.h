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

#ifndef FIFE_VIDEO_PARTIALACTION_H
#define FIFE_VIDEO_PARTIALACTION_H

// Standard C++ library includes
#include <vector>

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	class PartialAction;
	typedef ::boost::shared_ptr<PartialAction> PartialActionPtr;

	class PartialAction {
		public:
			enum { ANY_DIRECTION = 255 };

			PartialAction(uint8_t action, uint8_t number, uint8_t direction);
			~PartialAction();

			uint8_t getAction() const;
			uint8_t getNumber() const;
			uint8_t getDirection() const;
			
			void setFirstFrame(uint8_t frame);
			void setNumFrames(int8_t frame);

			uint8_t getFirstFrame() const;
			int8_t getNumFrames() const;

			void setWarpFactor(int16_t warpFactor);
			int16_t getWarpFactor() const;

			void addNextPartialAction(const PartialActionPtr& partialAction);
			const PartialActionPtr& findNextPartialAction(uint8_t action, uint8_t direction) const; 

			bool matches(uint8_t action, uint8_t direction) const;
		private:
			int16_t m_warpFactor;

			uint8_t m_action;
			uint8_t m_number;
			uint8_t m_direction;

			uint8_t m_firstFrame;
			int8_t m_numFrames;

			typedef std::vector<PartialActionPtr> type_partialActions;
			type_partialActions m_partialActions;
	};

	//////////////////// INLINE FUNCTIONS /////////////////////////////
	inline
	uint8_t PartialAction::getAction() const {
		return m_action;
	}

	inline
	uint8_t PartialAction::getNumber() const {
		return m_number;
	}

	inline
	uint8_t PartialAction::getDirection() const {
		return m_direction;
	}

	inline
	uint8_t PartialAction::getFirstFrame() const {
		return m_firstFrame;
	}

	inline
	int8_t PartialAction::getNumFrames() const {
		return m_numFrames;
	}

	inline
	int16_t PartialAction::getWarpFactor() const {
		return m_warpFactor;
	}

	inline
	void PartialAction::setWarpFactor(int16_t warpFactor) {
		m_warpFactor = warpFactor;
	}

	inline
	bool PartialAction::matches(uint8_t action, uint8_t direction) const {
		return
			action == m_action
			&& (direction == m_direction || m_direction == ANY_DIRECTION);
	}

} // NS: FIFE


#endif
