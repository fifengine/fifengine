/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#ifndef LUA_REF_H
#define LUA_REF_H

// Standard C++ library includes

// 3rd party library includes
#include "lua.hpp"

// FIFE includes

namespace FIFE {

	/** Utility class for objects that hold refernces to lua objects.
	 * @bug What happens if the state is destroyed and a reference is still hold?
	 */
	class LuaRef {
		public:
			/** Constructor
			 *  @note Initially @see isValid should return @c false
			 */
			LuaRef();

			/** Destructor
			 *  @note This will call unref. Just in case.
			 */
			~LuaRef();

			/** Reference a object on the lua stack
			 *  Grabs a refrence of an object on the lua stack,
			 *  and also saves the lua state for later reference.
			 *  @note This will call unref. Just in case.
			 *  @note After this call @see isValid should return @c true.
			 *  @param index Index of the element on the stack, that should be referenced.
			 *  @param L the current lua state.
			 */
			void ref(lua_State *L, int index);

			/** Release any reference held
			 *  @note After this call @see isValid will return @c false
			 */
			void unref();

			/** Check whether currentl a reference is held.
			 */
			bool isValid() const;

			/** Push the referenced object on the lua stack it was referenced in.
			 *  The refrenced object will be on the top of the current stack,
			 *  @note Do @b not call this if @c isValid returns @c false
			 */
			void push();

			/** Return the lua stack a lua object was referenced in.
			 *  @note Do @b not call this if @c isValid returns @c false
			 */
			lua_State* getState();

		protected:
			// The lua ref as returned by luaL_ref
			int m_ref;
			// The lua state the reference was opetained in
			lua_State* m_state;
	};

}

#endif
