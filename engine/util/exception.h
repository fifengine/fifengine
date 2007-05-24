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

#ifndef FIFE_EXCEPTION_H
#define FIFE_EXCEPTION_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	/** Exception base class.
	 *
	 * All other exceptions derived from this merely adjust the error string
	 * to be slightly more specific.
	 */
	class Exception {
		public:
			/** Constructor. 
			 *
			 * @param txt The error mesage to be stored.
			 */
			Exception(const std::string& txt);

			/** Destructor. 
			 */
			virtual ~Exception();

			/** Returns the error message.
			 *
			 * @return The error message.
			 */
			const std::string& getMessage() const;

		private:
			// The error string.
			std::string m_message;

	};

	// Common exceptions.

	/** SDL reported something bad :( 
	 */
	class SDLException : public Exception {
		public:
			SDLException(const std::string& msg) : Exception("SDLException: " + msg) {}
	};

	/** We searched for something, but it's missing in action.... 
	 */
	class NotFound : public Exception {
		public:
			NotFound(const std::string& msg) : Exception("NotFound: " + msg) {}
	};

	/** Someone tried to access a non-existing element.
	 */
	class IndexOverflow : public Exception {
		public:
			IndexOverflow(const std::string& s) : Exception("IndexOverflow: " + s) {}
	};

	/** Found invalid data.
	 */
	class InvalidFormat : public Exception {
		public:
			InvalidFormat(const std::string& s) : Exception("InvalidFormat: " + s) {}
	};

	/** We couldn't open a needed file. 
	 */
	class CannotOpenFile : public Exception {
		public:
			CannotOpenFile(const std::string& s) : Exception("CannotOpenFile: " + s) {}
	};

	/** Buy more ram ;)
	 *  @bug The memory allocation in @c std::string might fail, resulting in terminate.
	 */
	class OutOfMemory : public Exception {
		public:
			OutOfMemory(const std::string& s) : Exception("OutOfMemory: " + s) {}
	};

	/** Tried an invalid conversion. 
	 */
	class InvalidConversion : public Exception {
		public:
			InvalidConversion(const std::string& s) : Exception("InvalidConversion: " + s) {}
	};

	/** This action was not supported. 
	 */
	class NotSupported : public Exception {
		public:
			NotSupported(const std::string& s) : Exception("NotSupported: " + s) {}
	};

	/** A name or identifier is already in use
	 */
	class NameClash : public Exception {
		public:
			NameClash(const std::string& s) : Exception("Name already in use: " +s) {}
	};

	/** A duplicate item was added, where this is not allowed.
	 */
	class Duplicate : public Exception {
		public:
			Duplicate(const std::string& s) : Exception("Duplicate item: " +s) {}
	};

	/** Error related to scripting functionality
	 */
	class ScriptException : public Exception {
		public:
			ScriptException(const std::string& s) : Exception ( "Script-Exception: " +s) {};
	};

	/** Error related to event functionality
	 */
	class EventException : public Exception {
		public:
			EventException(const std::string& s) : Exception ( "Event Exception: " +s) {};
	};


}//FIFE

#endif
