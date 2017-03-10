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

#ifndef FIFE_EXCEPTION_H
#define FIFE_EXCEPTION_H

// Standard C++ library includes
#include <string>
#include <stdexcept>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"

namespace FIFE {

	/** Exception base class.
	 * All other exceptions derived from this merely adjust the error string
	 * to be slightly more specific.
	 */
	class Exception : public std::runtime_error {
	public:
		/** Constructor.
		 * @param msg The error mesage to be stored.
		 */
		Exception(const std::string& msg);

		/** Destructor.
		 */
		virtual ~Exception() throw();

		/** Returns the error message.
		 * @return The error message.
		 */
		virtual const char* what() const throw();

		virtual const std::string& getTypeStr() const { static const std::string s = "Exception"; return s; }
		virtual const std::string& getDescription() const { static const std::string s = "Generic FIFE exception"; return s; }
		// little helper to change m_what
		void update();

	private:
		std::string m_what;
	};

	#define FIFE_EXCEPTION_DECL(_name, _description) \
	class _name : public Exception { \
	public: \
		_name(const std::string& msg) : Exception(msg) { Logger _log(LM_EXCEPTION); update(); FL_ERR(_log, what()); } \
		const std::string& getTypeStr() const { static const std::string s = #_name; return s; } \
		const std::string& getDescription() const { static const std::string s = _description; return s; } \
	}

	FIFE_EXCEPTION_DECL(SDLException, "SDL reported something bad");
	FIFE_EXCEPTION_DECL(NotFound, "Something was searched, but not found");
	FIFE_EXCEPTION_DECL(NotSet, "Something was not set correctly");
	FIFE_EXCEPTION_DECL(IndexOverflow, "Someone tried to access a non-existing element");
	FIFE_EXCEPTION_DECL(InvalidFormat, "Found invalid data");
	FIFE_EXCEPTION_DECL(CannotOpenFile, "File couldn't be opened");
	FIFE_EXCEPTION_DECL(InvalidConversion, "Tried an invalid conversion");
	FIFE_EXCEPTION_DECL(NotSupported, "This action was not supported");
	FIFE_EXCEPTION_DECL(NameClash, "A name or identifier is already in use");
	FIFE_EXCEPTION_DECL(Duplicate, "A duplicate item was added, where this is not allowed");
	FIFE_EXCEPTION_DECL(ScriptException, "Error related to scripting functionality");
	FIFE_EXCEPTION_DECL(EventException, "Error related to event functionality");
	FIFE_EXCEPTION_DECL(GuiException, "Error related to gui functionality");
	FIFE_EXCEPTION_DECL(InconsistencyDetected, "An inconsistency in FIFE internals was detected. Please report this is a FIFE Bug.");

	/** @bug The memory allocation in @c std::string might fail, resulting in terminate. */
	FIFE_EXCEPTION_DECL(OutOfMemory, "Buy more ram ;)");


}//FIFE

#endif
