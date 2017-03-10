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

#ifndef FIFE_TIMER_H
#define FIFE_TIMER_H

// Standard C++ library includes

// 3rd party library includes
#include <boost/function.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "timeevent.h"

namespace FIFE {

	/** Simple Timer class
	 *
	 *  This is a simple wrapper around the TimeEvent,
	 *  which makes the later usable without having to
	 *  subclass it every time.
	 *
	 *  @code
	 *  m_timer.setInterval(1000);
	 *  m_timer.setCallback( boost::bind(&Class::update, this) );
	 *  m_timer.start();
	 *  @endcode
	 *
	 *  @warning Note that the callback must be available
	 *  when it is triggered. If your called function is a
	 *  bound member function and it goes out of scope,
	 *  when the callback is called you'll most likely
	 *  have a segfault. So this is best used as a member
	 *  calling an update function.
	 *
	 *  @note You don't have to call TimeManager::registerEvent @b ever
	 *
	 *  @see Console
	 */
	class Timer : protected TimeEvent {
	public:
		typedef boost::function0<void> type_callback;

		/** Default constructor.
		 *
		 *  Constructs an idle timer, use @see setInterval and @see setCallback
		 *  to set it up for use.
		 */
		Timer();

		/** Destructor.
		 *
		 *  Stops and destroys the timer.
		 */
		virtual ~Timer();

		/** Set the interval in milliseconds
		 *
		 *  @param msec The interval
		 *  If you use an intervall 0 the timer will fire every frame.
		 */
		void setInterval(int32_t msec);

		/** Start the timer
		 *
		 * Without calling this function, nothing will happen.
		 * It is save to call this more than once.
		 */
		void start();

		/** Stop the timer
		 *
		 *  Stops execution of the callback
		 *  It is save to call this more than once.
		 */
		void stop();

		/** Set the callback that will be called
		 *
		 *  @param callback A @c boost::function0 returning void
		 */
		void setCallback(const type_callback& callback);

	protected:
		bool m_active;
		type_callback m_callback;
		void updateEvent(uint32_t);
	};

}//FIFE

#endif
