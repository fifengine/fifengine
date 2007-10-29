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

%module fife
%{
#include "util/exception.h"
%}

namespace FIFE {
	class Exception {
	public:
		Exception(const std::string& txt);
		virtual ~Exception();
		const std::string& getMessage() const;
	};
	
	class SDLException : public Exception {
	public:
		SDLException(const std::string& msg) : Exception("SDLException: " + msg) {}
	};
	
	class NotFound : public Exception {
	public:
		NotFound(const std::string& msg);
	};
	class NotSet : public Exception {
	public:
		NotSet(const std::string& msg);
	};
	class IndexOverflow : public Exception {
	public:
		IndexOverflow(const std::string& s);
	};
	class InvalidFormat : public Exception {
	public:
		InvalidFormat(const std::string& s);
	};
	class CannotOpenFile : public Exception {
	public:
		CannotOpenFile(const std::string& s);
	};
	class OutOfMemory : public Exception {
	public:
		OutOfMemory(const std::string& s);
	};
	class InvalidConversion : public Exception {
	public:
		InvalidConversion(const std::string& s);
	};
	class NotSupported : public Exception {
	public:
		NotSupported(const std::string& s);
	};
	class NameClash : public Exception {
	public:
		NameClash(const std::string& s);
	};
	class Duplicate : public Exception {
	public:
		Duplicate(const std::string& s);
	};
	class ScriptException : public Exception {
	public:
		ScriptException(const std::string& s);
	};
	class EventException : public Exception {
	public:
		EventException(const std::string& s);
	};
	class GuiException : public Exception {
	public:
		GuiException(const std::string& s);
	};

}