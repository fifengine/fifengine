/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#include <string>

namespace FIFE {

	/** Baseclass for all exceptions thrown by FIFE */
	class Exception {
		public:
			Exception(const std::string& txt);
			virtual ~Exception();

			/** Returns the error message
				*
				* @return error message
				*/
			const std::string& getMessage() const;

		private:
			std::string m_message;

	};

	// common exceptions

	/** SDL reported something bad :( */
	class SDLException : public Exception {
		public:
			SDLException(const std::string& msg) : Exception("SDLException: " + msg) {}
	};

	/** We searched for something, but it's missing in action.... */
	class NotFound : public Exception {
		public:
			NotFound(const std::string& msg) : Exception("NotFound: " + msg) {}
	};

	/** Someone tried to access a non-existing element */
	class IndexOverflow : public Exception {
		public:
			IndexOverflow(const std::string& s) : Exception("IndexOverflow: " + s) {}
	};

	/** Found invalid data */
	class InvalidFormat : public Exception {
		public:
			InvalidFormat(const std::string& s) : Exception("InvalidFormat: " + s) {}
	};

	/** We couldn't open a needed file */
	class CannotOpenFile : public Exception {
		public:
			CannotOpenFile(const std::string& s) : Exception("CannotOpenFile: " + s) {}
	};

	/** Buy more ram ;) */
	class OutOfMemory : public Exception {
		public:
			OutOfMemory(const std::string& s) : Exception("OutOfMemory: " + s) {}
	};

	/** Tried a invalid conversion */
	class InvalidConversion : public Exception {
		public:
			InvalidConversion(const std::string& s) : Exception("InvalidConversion: " + s) {}
	};

}//FIFE

#endif
