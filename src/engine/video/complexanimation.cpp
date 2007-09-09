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

#include "complexanimation.h"
#include "animation.h"

namespace FIFE {

	ComplexAnimation::ComplexAnimation() :RenderAble(RT_COMPLEX_ANIMATION){
		nowAction = nowDirection = 0;
	}
	ComplexAnimation::~ComplexAnimation() {
		cleanup();
	}
	void ComplexAnimation::cleanup() {
		T_animByAction::iterator i = content.begin();
		while (i != content.end()) {
			T_animByDirection::iterator j = i->second.begin();
			while (j != i->second.end()) {
				delete j->second;
				j++;
			}
			i->second.clear();
			i++;
		}
		content.clear();
	}
	void ComplexAnimation::registerAnimation(uint8_t action, uint8_t direction, Animation *anim) {
		T_animByAction::iterator i = content.find(action);
		// thow if i == content.end()
		T_animByDirection::iterator j = i->second.find(direction);
		// throw if j == i.second.end()
		assert(anim != NULL);
		j->second = anim;
	}
	void ComplexAnimation::setup(uint8_t startAction, uint8_t startDirection) {
		nowAction = startAction;
		nowDirection = startDirection;
		// actually find it?
		activeAnimation = content.find(nowAction)->second.find(nowDirection)->second;
	}
	void ComplexAnimation::setDirection(uint8_t directionId) {
		// check if it exists
		T_animByAction::iterator i = content.find(nowAction);
		T_animByDirection::iterator j = i->second.find(directionId);
	}
	void ComplexAnimation::setAction(uint8_t actionId) {
		T_animByAction::iterator i = content.find(actionId);
		activeAnimation = i->second.find(nowDirection)->second;
	}

	int ComplexAnimation::getXShift() const {
		return 0;//m_shifts_x[currentFrame];
	}
	int ComplexAnimation::getYShift() const {
		return 0;//m_shifts_y[currentFrame];
	}
	void ComplexAnimation::setXShift(int xshift) {
		//m_shifts_x[currentFrame] = xshift;
	}
	void ComplexAnimation::setYShift(int yshift) {
		//m_shifts_y[currentFrame] = yshift;
	}
}; // FIFE
