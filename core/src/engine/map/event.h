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

#ifndef FIFE_MAP_EVENT_H
#define FIFE_MAP_EVENT_H

// Standard C++ library includes
#include <cassert>

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "exception.h"
#include "timemanager.h"

namespace FIFE { namespace map {
	
	enum eventCodes {
		FIFE_EXECCOMMAND = 1,
		FIFE_HEARTBEAT,
		FIFE_SHUTDOWN,
		FIFE_EXEC,
		FIFE_EXEC_FILE,
		FIFE_EXEC_STRING
	};

	namespace event_detail {
/*
	Intrusive pointer impl. would be ~2 times faster than shared_ptr
	but I am unsure about thread safety in the case of an event copy
	existing in one thread and one in the other:
	It depends on ++m_ref being atomic, if it is equal to:

		i = m_ref
		i = i + 1
		m_ref = i

	Then we're screwed.

	Nevertheless, the shared_ptr impl. is very fast, too.
*/

// 		class Payload { 
// 				long m_ref;
// 			public:
// 				Payload() : m_ref(1) {};
// 				virtual ~Payload(){};
// 
// 				void ref() { ++m_ref; };
// 				void unref() { --m_ref; if(0==m_ref) delete this; };
// 		};
//		typedef Payload* PayloadPtr;

		class Payload { 
			public:
				virtual ~Payload(){};
		};
		typedef boost::shared_ptr<Payload> PayloadPtr;

		template<typename T>
		class PayloadWrapper : public Payload {
			public:
				typedef T type_data;
				type_data data;
	
				PayloadWrapper(const type_data& d) : data(d) {}
		};
	}

	/** Events for the AsyncBridge
	 *
	 *  Events are @b read-only structures, created @b once and after
	 *  that they are only read or copied.
	 *
	 *  A payload of an copy-constructible type may be added, this will
	 *  be copied once and then a pointer to it is maintained by all
	 *  instances of the event.
	 *
	 *  Is thread safe.
	 */
	class event_t {
			event_t() {}

		public:
			event_t(uint8_t _type, uint16_t _code)
				: m_data() {

				m_type = _type;
				m_code = _code;

				// This IS actually faster than SDL_GetTicks =)
				m_ticks = TimeManager::instance()->getTime();
			}

			event_t(uint8_t _type, uint16_t _code,
			        event_detail::PayloadPtr _data)
				: m_data(_data) {

				m_type = _type;
				m_code = _code;

				// This IS actually faster than SDL_GetTicks =)
				m_ticks = TimeManager::instance()->getTime();
			}
	
			event_t(const event_t& e)
				: m_type(e.m_type),
				  m_code(e.m_code),
				  m_ticks(e.m_ticks),
				  m_data(e.m_data) {
// 				if (m_data) m_data->ref();
			}
	
			/** Destructor
			 */
			~event_t() {
// 				if (m_data) m_data->unref();
			}
	
			/** Access the payload
			 */
			template<typename T>
			const T& get() const;
	
			/// Event type
			uint8_t type() const { return m_type; };
	
			/// Event code
			uint16_t code() const { return m_code; };

			/// Time of creation
			uint32_t ticks() const { return m_ticks; };

		private:
			uint8_t  m_type;
			uint8_t  m_align;
			uint16_t m_code;
			uint32_t m_ticks;
			event_detail::PayloadPtr m_data;
	};

	////////////////// INLINE UTIITY FUNCTIONS ////////////////////

	template<typename T>
	inline
	event_t makeEvent(uint16_t _code, const T& _data) {
		event_detail::PayloadPtr payload(new event_detail::PayloadWrapper<T>(_data));
		return event_t(0,_code,payload);
	}

	template<typename T>
	inline
	event_t makeEvent(uint16_t _code, uint32_t _addr,uint32_t _source, const T& _data) {
		event_detail::PayloadPtr payload(new event_detail::PayloadWrapper<T>(_data));
		return event_t(0,_code,payload);
	}

	/////////// INLINE MEMBER FUNCTIONS /////////////////////

	template<typename T>
	inline
	const T& event_t::get() const {
		event_detail::PayloadWrapper<T> *payload;
		payload = dynamic_cast<event_detail::PayloadWrapper<T>*>(m_data.get());
		if( payload == 0 ) {
			throw InvalidConversion(typeid(m_data).name());
		}
		return payload->data;
	}

} }

#endif
