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

#ifndef FIFE_POSSIBLESUBNAMESPACES_FOO_H
#define FIFE_POSSIBLESUBNAMESPACES_FOO_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	// FORWARD REFERENCES

	/**  A one line description of the class.  
	 *
	 * A longer description.
	 *
	 * @see something
	 */
	class Foo {
	public:

		// LIFECYCLE
		/** Default constructor.
		 */
		Foo();

		/** Copy constructor.
		 * 
		 * @param from The value to copy to this object.
		 */
		Foo(const Foo& from);

		/** Destructor.
		 */
		~Foo();

		// OPERATORS
		/** Assignment operator.
		 *
		 * @param from The value to assign to this object.
		 *
		 * @return A reference to this object.
		 */
		Foo& operator=(Foo& from);

	// OPERATIONS
	// ACCESS
	// INQUIRY

	protected:
	private:
	};

	// INLINE METHODS
	//

	// EXTERNAL REFERENCES
	//

} //FIFE

#endif
