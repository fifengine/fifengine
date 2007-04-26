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
#include "map/geometry.h"
#include "util/debugutils.h"
#include "util/exception.h"
#include "imagecache.h"
#include "util/time/timemanager.h"

#include "animation.h"
#include "complexanimation.h"
#include "partialaction.h"
#include "util/rect.h"

namespace FIFE {

	ComplexAnimation::UpdateEvent::UpdateEvent(ComplexAnimation* cmplxAnim)
		: TimeEvent(-1),
		m_complexAnimation(cmplxAnim) {
		m_lastTime = TimeManager::instance()->getTime();
	}

	ComplexAnimation::UpdateEvent::~UpdateEvent() {
		TimeManager::instance()->unregisterEvent(this);
	}

	void ComplexAnimation::UpdateEvent::updateEvent(unsigned long time) {
		m_complexAnimation->update( time - m_lastTime );
		m_lastTime = time;
	}

	ComplexAnimation::ComplexAnimation(const std::string& name) 
		: RenderAble(RT_COMPLEX_ANIMATION),
		m_warpOffsets(),
		m_actionsPtr( new type_actions ),
		m_name(name),
		m_updateEvent(new UpdateEvent(this)) {
		initState();
	}

	ComplexAnimation::ComplexAnimation(const ComplexAnimation& master) 
		: RenderAble(RT_COMPLEX_ANIMATION),
		m_warpOffsets(),
		m_actionsPtr( master.m_actionsPtr ),
		m_name(master.m_name),
		m_updateEvent(new UpdateEvent(this)) {
		initState();
	}

	ComplexAnimation::~ComplexAnimation() {
		delete m_updateEvent;
	}

	void ComplexAnimation::registerAnimation(uint8_t action, uint8_t direction, size_t animation) {
		if( m_actionsPtr->size() <= action ) {
			m_actionsPtr->resize(action + 1);
		}

		s_action& actionEntry = m_actionsPtr->at( action );
		if( actionEntry.animations.size() <= direction ) {
			actionEntry.animations.resize(direction + 1);
		}

		actionEntry.animations[ direction ] = animation;
	}

	void ComplexAnimation::registerPartialAction(const PartialActionPtr& partialAction) {
		uint8_t action = partialAction->getAction();
		if( m_actionsPtr->size() <= action ) {
			m_actionsPtr->resize(action + 1);
		}
		m_actionsPtr->at( action ).partialActions.push_back( partialAction );
	}

	void ComplexAnimation::enqueueAction(const map::Action& queue_entry) {
		m_actionQueue.push_back( queue_entry );
		if( m_state.animation == ImageCache::UNDEFINED_ID ) {
			activateNextAction();
		}
	}

	void ComplexAnimation::emptyQueue() {
		m_actionQueue.clear();
	}

	bool ComplexAnimation::activateNextAction() {
// 		DEBUG_PRINT("activateNextAction: " << m_name);
		do {
			if( m_actionQueue.empty() ) {
				// Deactivate updating
				// and continue showing the
				// last frame
				m_updateEvent->setPeriod(-1);
				return false;
			}
			m_state.action    = m_actionQueue.front().actionId;
			m_state.direction = m_actionQueue.front().direction;
			m_state.curAction = m_actionQueue.front();
			m_actionQueue.pop_front();

		} while( !chooseNextPartialAction( m_state.action, m_state.direction ) );
// 		DEBUG_PRINT("found suitable next action!")
		Animation* animation = 0;
		m_state.animation    = 0;

		// The following code tries to be as safe as possible against defunkt
		// XML data. The Provider does not check any-/everything, so we have to
		// be carefull ;-)

		if( m_actionsPtr->size() > m_state.action ) {
			m_state.animation = 
				m_actionsPtr->at( m_state.action ).animations[ m_state.direction ];
		}
		try {
			animation = dynamic_cast<Animation*>(
				ImageCache::instance()->getImage(m_state.animation));
		} catch( IndexOverflow& e ) {
			animation = 0;
		}

		if( animation == 0 ) {
			Warn("complex_animation")
				<< "Warning: action("
				<< m_name << ") without an animation? "
				<< "Will try to ignore it.";

			// We could in principle remove the offender
			// but this won't really fix the bug in the data file
			// So we try to move on.
			if( !m_actionQueue.empty() ) {
				activateNextAction();
			} else {
				m_state.animation = 0;
			}
			return false;
		}

		m_state.frameDuration = animation->getFrameDuration();
		m_state.numFrames  = animation->getNumFrames();


		m_state.frame = m_state.partialAction->getFirstFrame();
		m_state.frameCounter = m_state.partialAction->getNumFrames();
		m_state.animDir = 1;
		if (m_state.frameCounter < 0) {
			m_state.animDir = -1;
			m_state.frameCounter *= -1;
		}

		// Wrap around
		m_state.frame %= m_state.numFrames;

		if( m_warpOffsets.size() > m_state.direction ) {
			m_state.warpShift = m_warpOffsets[ m_state.direction ];
		} else {
			m_state.warpShift = Point();
		}

		calculateScreenBox(animation);
		m_updateEvent->setPeriod((3*m_state.frameDuration)/4 );

		// Call the Action start callback.
		if (m_state.curAction.startCallback)
			m_state.curAction.startCallback(m_state.curAction);

// 		DEBUG_PRINT("CA: animation phase set "
// 			<< "animation: " << int(m_state.animation)
// 			<< " frames: " << int(m_state.firstFrame) 
// 			<< "->" << int(m_state.lastFrame));

		// Don't touch the accumulated time in m_state.frameTime
		// this is used to jump over actions by update() in the
		// case of a very long timeDelta.
		return true;
	}

	void ComplexAnimation::setActiveAction(const map::Action& action, bool delayOverride) {
		if( delayOverride ) {
			m_state.frameTime = 0;
		}
		// We use a trick here to save some typing ^^
		// Clear the queue, set the current action to zero
		// and call enqueueAction()

		if (m_state.curAction.endCallback)
			m_state.curAction.endCallback(m_state.curAction);

		m_state.partialAction.reset();
		m_actionQueue.clear();
		m_state.animation = ImageCache::UNDEFINED_ID;
		enqueueAction(action);
	}

	bool ComplexAnimation::chooseNextPartialAction(uint8_t action, uint8_t direction) {
//		DEBUG_PRINT("chooseNextPartialAction: " 
//			<< int(action) << "(" << int(direction) << ")");
		// action and direction is the now
		// current action -> choose the partial action for it.

		// Try to find a fitting continuation of the current animation
		if( m_state.partialAction ) {
			m_state.partialAction =
				m_state.partialAction->findNextPartialAction(action, direction);
		}

		// None found, or no current partialAction?
		// Try using the one with number=="0"
		if( !m_state.partialAction ) {
			m_state.partialAction = findFirstPartialAction(action,direction);
		}

		if( !m_state.partialAction ) {
			Debug("complex_animation")
				<< "No suiting partial action found ("
				<< m_name
				<< ") action:" << int(action)
				<< "(" << int(direction) << ")";
			return false;
		}
		return true;
	}

	const PartialActionPtr& ComplexAnimation::findFirstPartialAction(uint8_t action,uint8_t direction) const {
		static PartialActionPtr zeroPtr;
		if( m_actionsPtr->size() <= action )
			return zeroPtr;

		const s_action& actionEntry = m_actionsPtr->at( action );
		for(size_t i = 0; i != actionEntry.partialActions.size(); ++i) {
			if( 0 == actionEntry.partialActions[i]->getNumber() ) {
				return actionEntry.partialActions[i];
			}
		}
		return zeroPtr;
	}

	void ComplexAnimation::update(int32_t timeDelta) {

		m_state.frameTime += timeDelta;
		if( m_state.frameTime < m_state.frameDuration ) {
			return;
		}
		bool change_period =  true;

		while (m_state.frameTime >= m_state.frameDuration) {
			m_state.frameTime -= m_state.frameDuration;
			m_state.frameCounter--;

			// Advance a frame
			m_state.frame += m_state.animDir;

			// Wrap around
			if (m_state.frame < 0) 
				m_state.frame = m_state.numFrames - 1;
			if (m_state.frame > m_state.numFrames - 1) {
				m_state.frame = 0;
				m_state.warpShift = Point();
			}
	
			if (!m_state.frameCounter) { // This action is finished!

				// Call Actions endCallback
				if (m_state.curAction.endCallback)
					m_state.curAction.endCallback(m_state.curAction);

				// Choose next action.	
				change_period = activateNextAction();
			}
		}
		if (change_period)
			m_updateEvent->setPeriod( m_state.frameDuration - m_state.frameTime );


		// It occured that animation was null here ... not sure exactly why, so
		// we check it anyway. But we do need a calculateScreenbox().
		Animation* animation = dynamic_cast<Animation*>(ImageCache::instance()->getImage(m_state.animation));
		if (animation && m_state.partialAction)
			calculateScreenBox(animation);
	}

	void ComplexAnimation::render(const Rect& rect, Screen* screen, uint8_t alpha) {
// 		DEBUG_PRINT("CA: render start: " << int(m_state.animation) );
		if( !m_state.animation )
			return;
		Animation* animation = dynamic_cast<Animation*>(
			ImageCache::instance()->getImage(m_state.animation));
		if( !animation )
			return;

		Rect target(rect);

		RenderAble* r = animation->getFrame(m_state.frame);

		target.x += m_state.shift.x;
		target.y += m_state.shift.y;

		r->render(target,screen,alpha);
		
// 		DEBUG_PRINT("CA: render end " << target);
	}

	void ComplexAnimation::initState() {
		m_state.animation = ImageCache::UNDEFINED_ID;
		m_state.frame = 0;
		m_state.frameTime = 0;
		m_state.warpShift = Point(); // Necessary?
		TimeManager::instance()->registerEvent(m_updateEvent);
	}

	void ComplexAnimation::calculateScreenBox(Animation* animation) {
		int16_t warpFactor = - m_state.partialAction->getWarpFactor();
		m_state.shift.x = animation->getFrame(m_state.frame)->getXShift();
		m_state.shift.y = animation->getFrame(m_state.frame)->getYShift();
		m_state.shift.x += animation->getXShift();
		m_state.shift.y += animation->getYShift();
		m_state.shift   += m_state.warpShift * warpFactor;
		m_state.size.x  = animation->getFrame(m_state.frame)->getWidth();
		m_state.size.y  = animation->getFrame(m_state.frame)->getHeight();
	}

	void ComplexAnimation::resetGeometry(map::Geometry *geometry) {
		assert(geometry);
		size_t numDirections = geometry->getNumDirections();

		m_warpOffsets.resize(numDirections);
		for(size_t i = 0; i != numDirections; ++i) {
			m_warpOffsets[i] = geometry->directionToScreen(i);
		}

		Animation* animation = dynamic_cast<Animation*>(ImageCache::instance()->getImage(m_state.animation));
		if (animation && m_state.partialAction)
			calculateScreenBox(animation);
// 		Warn("cap") << __FUNCTION__ << ": " << m_state.size;
	}

	unsigned ComplexAnimation::getWidth() const {
		return m_state.size.x;//m_shifts_x[currentFrame];
	}
	unsigned ComplexAnimation::getHeight() const {
		return m_state.size.y;//m_shifts_y[currentFrame];
	}
	int ComplexAnimation::getXShift() const {
		return 0;//m_state.shift.x;//m_shifts_x[currentFrame];
	}
	int ComplexAnimation::getYShift() const {
		return 0;//m_state.shift.y;//m_shifts_y[currentFrame];
	}

	void ComplexAnimation::setXShift(int xshift) {
	}
	void ComplexAnimation::setYShift(int yshift) {
	}


}; // FIFE
