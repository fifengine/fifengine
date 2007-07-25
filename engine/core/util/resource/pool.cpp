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
#include "resource_provider.h"

namespace FIFE {
	template <typename TResource> 
	Pool<TResource>::Pool(): 
		m_pooledobjs(),
		m_entries(),
		m_listeners(),
		m_providers(),
		m_curind(0)
	{
	}

	template <typename TResource> 
	Pool<TResource>::~Pool() {
		clear();
		typename std::vector<IResourceProvider<TResource>*>::iterator provider;
		for (provider = m_providers.begin(); provider != m_providers.end(); provider++) {
			delete (*provider);
		}

	}
	
	template <typename TResource> 
	void Pool<TResource>::addResourceProvider(IResourceProvider<TResource>* provider) {
		m_providers.push_back(provider);
	}

	template <typename TResource> 
	int Pool<TResource>::addResourceFromLocation(const ResourceLocation& obj) {
		return 0;
	}

	template <typename TResource> 
	TResource& Pool<TResource>::get(int index) const {
		return *(m_entries[index].resource);
	}

	template <typename TResource> 
	void Pool<TResource>::clear() {
		std::vector<PoolListener*>::iterator listener;
		for (listener = m_listeners.begin(); listener != m_listeners.end(); listener++) {
			(*listener)->poolCleared();
		}

		typename std::map<int, TResource*>::iterator obj;
        	for (obj = m_pooledobjs.begin(); obj != m_pooledobjs.end(); ++obj) {
			delete *(obj->second);
 		}

		m_pooledobjs.clear();
	}

	template <typename TResource> 
	void Pool<TResource>::addPoolListener(PoolListener* listener) {
		m_listeners.push_back(listener);
	}

	template <typename TResource> 
	void Pool<TResource>::removePoolListener(PoolListener* listener) {
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
