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

#ifndef FIFE_BACKENDMANAGER_H
#define FIFE_BACKENDMANAGER_H

// Standard C++ library includes
#include <map>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "exception.h"

namespace FIFE {

	template <typename KEY, class BACKEND> class BackendManager {
		public:
			typedef std::vector<KEY> type_keylist;
			
			BackendManager() : m_current(0) {}

			virtual ~BackendManager() {
				cleanup();
			}

			virtual bool exists(const KEY& key) const {
				return m_backends.find(key) != m_backends.end();
			}

			virtual void add(const KEY& key, BACKEND* backend) {
				m_backends[key] = backend;
			}

			virtual BACKEND* current() const {
				return m_current;
			}

			virtual BACKEND* get(const KEY& key) const {
					typename type_backends::const_iterator i = m_backends.find(key);
					if (i == m_backends.end())
						throw NotFound("couldn't find requested backend");

					return i->second;
				}

			virtual void select(const KEY& key) {
				BACKEND* newbackend = get(key);
				if (m_current)
					m_current->deinit();

				m_current = newbackend;
				m_current->init();
			}

			virtual type_keylist keys() const {
				type_keylist list;
				typename type_backends::const_iterator end = m_backends.end();
				for (typename type_backends::const_iterator i = m_backends.begin(); i != end; ++i)
					list.push_back(i->first);

				return list;
			}

		private:
			typedef std::map<KEY, BACKEND*> type_backends;
			type_backends m_backends;

			BACKEND* m_current;

			void cleanup() {
				if (m_current)
					m_current->deinit();
				
				typename type_backends::const_iterator end = m_backends.end();
				for (typename type_backends::iterator i = m_backends.begin(); i != end; ++i)
					delete i->second;

				m_backends.clear();
			}
	};

}

#endif
