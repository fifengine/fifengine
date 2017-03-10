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

%module fife
%{
#include "util/base/exception.h"
#include "vfs/vfs.h"
%}

%include "vfs/raw/rawdata.i"

namespace std{
	%template(StringSet) set<std::string>;
}

namespace std {
	%template(vectoru) vector<uint8_t>;
};


namespace FIFE {
	class VFS {
	public:

		VFS();
		virtual ~VFS();

		void cleanup();

		void addNewSource(const std::string& path);
		void removeSource(const std::string& path);

		bool exists(const std::string& file) const;
		RawData* open(const std::string& path);

		std::set<std::string> listFiles(const std::string& path) const;
		std::set<std::string> listDirectories(const std::string& path) const;
	};
}

