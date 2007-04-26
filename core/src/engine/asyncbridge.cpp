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
#include <iostream>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/event.h"

#include "asyncbridge.h"
#include "util/log.h"

namespace FIFE {

	int AsyncBridge::all_losses = 0;

	AsyncBridge::AsyncBridge() :
		qlen(0),
		src(0)
	{
		/* our  queue */
		a_send_lock = SDL_CreateMutex();
	
		/* other queue */
		a_recv      = 0;
		a_recv_lock = 0;
	
		a_shutdown  = false;

		m_queue_len = 0;
	}
	
	AsyncBridge::~AsyncBridge() {
		// lock the remote side and set it to unconnected
		if( src ) {
			SDL_mutexP( a_recv_lock );
			src->src = 0;
			SDL_mutexV( a_recv_lock );
		}
		
		SDL_DestroyMutex(a_send_lock);

		// clean up remaining items in the queue
		clearQueue();
	}

	void AsyncBridge::clearQueue() {
		qlen -= a_send.size();

		while( !a_send_buf.empty() ) a_send_buf.pop();
		while( !a_send.empty() ) a_send.pop();

		all_losses += qlen;
		Log( "asyncbridge" ) 
			<< "got " << m_queue_len
			<< " remaining elements at destruction time."
			<< " (total: " << all_losses << ")";
		qlen = 0;
	}

	size_t AsyncBridge::getQueueSize() const {
		return m_queue_len;
	}

	//TODO: locking might be off
	void AsyncBridge::setSource(AsyncBridge* _src) {
		// make sure that the remote isn't already connected otherwise
		assert( _src->src == 0 || _src->src == this );

		// now check self
		assert( src == 0 || src == _src );

		a_recv      = &_src->a_send;
		a_recv_lock = _src->a_send_lock;
		
		src = _src;
		_src->src = this;
	}

	// if we are not connected, the message should be @b buffered
	void AsyncBridge::sendEvent(const map::event_t& e) {
		++m_queue_len;
		a_send_buf.push(e);
	}
	
	int AsyncBridge::sendEvents() {
		/* send pending events all at once. */
		int num_events = 0;

		if( 0 == src ) {
			return 0;
		}

		if( a_send_buf.empty()) {
			return 0;
		}
		
		SDL_mutexP(a_send_lock);
		while(!a_send_buf.empty()) {
			a_send.push(a_send_buf.front());
			a_send_buf.pop();
			++num_events;
		}
		SDL_mutexV(a_send_lock);
		m_queue_len -= num_events;

		qlen += num_events;
		return num_events;
	}
					
	int AsyncBridge::processEvents() {
		if(!a_recv) return -1;
		int num_events = 0;

		// if we are not connected, the remote is dead and reading from
		// it will do bad things.
		if( 0 == src ) {
			return 0;
		}

		SDL_mutexP(a_recv_lock);
		while(!a_recv->empty()) {
			processEvent( a_recv->front() );
			a_recv->pop();
			++num_events;
		}
		SDL_mutexV(a_recv_lock);

		qlen -= num_events;
	
		if(a_shutdown) {
			return -1;
		} else {
			return num_events;
		}
	}
	
	void AsyncBridge::processEvent(const map::event_t& e) {
		switch(e.code()) {
			case map::FIFE_SHUTDOWN:
				a_shutdown = true;
				break;
			default:
				Warn("asyncbridge") 
					<< "AsyncBridge::processEvent(e)"
					<< " unhandled event (" << e.code() << ")";
		}
	}
	
	int AsyncBridge::sendShutdown() {
		map::event_t e = map::event_t(0,map::FIFE_SHUTDOWN);
	
		sendEvent(e);
		sendEvents();

		return 0;
	}

}
