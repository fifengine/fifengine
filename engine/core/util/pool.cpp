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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "pool.h"

namespace FIFE {
	template <typename T> Pool<T>::Pool(): m_curind(0) {
	}

	template <typename T> Pool<T>::~Pool() {
	}

	template <typename T> int Pool<T>::add(T* obj) {
		m_pooledobjs[m_curind++] = obj;
		return m_curind - 1;
	}

	template <typename T> T& Pool<T>::get(int index) const {
		return *m_pooledobjs[index];
	}

	template <typename T> void Pool<T>::clear() {
		typename std::map<int, T*>::iterator i;
        	for (i = m_pooledobjs.begin(); i != m_pooledobjs.end(); ++i) {
			delete *(i->second);
 		}
		m_pooledobjs.clear();
	}

	template <typename T> void Pool<T>::addPoolListener(PoolListener* listener) {
		m_listeners.push_back(listener);
	}

	template <typename T> void Pool<T>::removePoolListener(PoolListener* listener) {
		std::vector<PoolListener*>::iterator i = m_listeners.begin();
		while (i != m_listeners.end()) {
			if ((*i) == listener) {
				m_listeners.erase(i);
				return;
			}
			++i;
		}
	}


}
