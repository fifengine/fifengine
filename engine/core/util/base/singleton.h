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

#ifndef FIFE_SINGLETON_H
#define FIFE_SINGLETON_H

#define SINGLEFRIEND(classname) friend class FIFE::StaticSingleton<classname>; classname(); virtual ~classname();

// Standard C++ library includes
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "fifeclass.h"

namespace FIFE {

	/** The "classic" Singleton.
	 *
	 * @see DynamicSingleton
	 */
	template <typename T> class StaticSingleton {
		public:

			static T* instance() {
				static T inst;
				return &inst;
			}

		protected:

			StaticSingleton() {
			}

			virtual ~StaticSingleton() {
			}

		private:
			StaticSingleton(const StaticSingleton<T>&) {}
			StaticSingleton<T>& operator=(const StaticSingleton<T>&) {
				return this;
			}

	};

	/** Another Singleton.
	 *
	 * This implementations needs to be created and destroyed explicitly.
	 * That way the order of construction and destruction is well defined, so we don't
	 * get those nasty static initialization/destruction order problems.
	 *
	 * Engine will create all standard FIFE Singletons when created (just call Engine::instance())
	 * and destroy them on exit.
	 *
	 * Maybe we'll change this one day to use one of those funny NiftyCounter implementations.
	 *
	 * @see StaticSingleton
	 * @see Engine
	 */
	template <typename T> class DynamicSingleton {
		public:
			static T* instance() {
				assert(m_instance);
				return m_instance;
			}

			DynamicSingleton() {
				assert(!m_instance);
				m_instance = static_cast<T*>(this);
			}

			virtual ~DynamicSingleton() {
				m_instance = 0;
			}

		private:
			static T* m_instance;

			DynamicSingleton(const DynamicSingleton<T>&) {};
			DynamicSingleton<T&> operator=(const DynamicSingleton<T>&) {};

	};

	template <typename T> T* DynamicSingleton<T>::m_instance = 0;

}//FIFE

#endif
