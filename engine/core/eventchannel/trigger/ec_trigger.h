/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_EVENTCHANNEL_TRIGGER_H
#define FIFE_EVENTCHANNEL_TRIGGER_H

// Standard C++ library includes
//
#include <list>

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//

#include "ec_itriggercontroller.h"
#include "ec_itriggerlistener.h"

namespace FIFE {

  /**
  * A trigger is fired when certain preconditions met
  */
  class Trigger : public ITriggerController {

  public:

	Trigger(ITriggerListener& listener){
	  registerListener(listener);
	}

	Trigger() { };

	virtual ~Trigger(){
	  m_triggers.clear();
	}

	void pollTrigger(){
	  if(requirementsMet()){
		pollTriggers();
	  }
	}

	virtual bool requirementsMet(){
	  return false;
	}

	void registerTrigger(Trigger& trigger){
	  m_triggers.push_back(&trigger);
	}

	void unregisterTrigger(Trigger& trigger){
	  m_triggers.remove(&trigger);
	}

	void pollTriggers(){
	  for (std::list<Trigger*>::iterator it = m_triggers.begin(); it!=m_triggers.end(); ++it) {
		(*it)->pollTrigger();
	  }
	}

	void fireTrigger(){
	  for (std::list<ITriggerListener*>::iterator it = m_listeners.begin(); it!=m_listeners.end(); ++it) {
		(*it)->triggerFired();
	  }
	}

	void registerListener(ITriggerListener& triggerlistener){
	  m_listeners.push_back(&triggerlistener);
	}

	void unregisterListener(ITriggerListener& triggerlistener){
	  m_listeners.remove(&triggerlistener);
	}

  protected:
	std::list<ITriggerListener*> m_listeners;
	std::list<Trigger*> m_triggers;
  };
}

#endif
