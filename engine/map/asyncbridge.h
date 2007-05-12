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

#ifndef FIFE_ASYNCBRIDGE_H
#define FIFE_ASYNCBRIDGE_H

// Standard C++ library includes
#ifndef WIN32
#	include <unistd.h>
#endif
#include <deque>
#include <queue>


// 3rd party library includes
#include <SDL_mutex.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/event.h"

namespace FIFE {

	namespace map { class event_t; }
	
	class AsyncBridge {
		public:
			AsyncBridge();
			virtual ~AsyncBridge();

			/** Make an instance be a receiver
			 *
			 * The calling object is made a receiver for events
			 * from \p _src , making processEvents consume events
			 * from that object's send queue.
			 *
			 * This method MUST NOT be called on an already
			 * connected object!
			 *
			 * \param _src Source to connect to
			 *
			 * \internal
			 * I assume a 1:1 relationship, so no multiple senders
			 * or receivers.
			 * */
			void setSource(AsyncBridge* _src);
	
			/** Process events in in-queue 
			 *  returns the number of processed events or -1 to indicate shutdown 
			 */
			int processEvents();

			/**  Send events in buffer
			 *  return the number of sent events
			 *  (the number of events in the send_buf queue).
			 */
			int sendEvents();

			/** signal the other side of the bridge to shut down 
			 */
			int sendShutdown();
			
			/** Return number of buffered events
			 *  @warning Don't use this value apart from debugging
			 */
			size_t getQueueSize() const;

		protected:
			typedef std::queue<map::event_t,std::deque<map::event_t> > type_event_queue;

			SDL_mutex* a_send_lock;
			type_event_queue a_send;
			type_event_queue a_send_buf;
	
			SDL_mutex* a_recv_lock;
			type_event_queue* a_recv;
	
			bool a_shutdown;

			// the loss count is broken
			int qlen; // length of the local queue
			static int all_losses; // cumulative count of losses

			size_t m_queue_len;

			/** Connected "remote" side
			 *
			 * When setSource is called, both the sender's and the
			 * receiver's src attribute is set to point to the
			 * other side. When one side connects, this is reset
			 * to be a NULL pointer.
			 *
			 * \internal
			 * See setSource remark. This can only handle 1:1
			 * relationships.
			 * */
			AsyncBridge* src;
	
			virtual void processEvent(const map::event_t& e);

			/** Queue a single event
			 *
			 * The event \p e is scheduled for addition to the
			 * outgoing queue. A number of events can be sent
			 * by this method before sendEvents is called.
			 *
			 * If the object is not connected to a receiver, then
			 * \p e is deleted and dropped.
			 *
			 * \param e An event to be sent
			 *
			 * \internal
			 * Should this throw an exception or return an error
			 * code in case there is no receiver?
			 * */
			void sendEvent(const map::event_t& e);

			void clearQueue();
	};
}

#endif
