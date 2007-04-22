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
#include "vfssource.h"
#include "vfs.h"

#include <algorithm>

namespace FIFE {

	VFSSource::VFSSource() {}


	VFSSource::~VFSSource() {
		VFS::instance()->removeSource(this);
	}


}

std::string FIFE::VFSSource::fixPath(std::string path) const
{
	if (!path.size())
		return path;

	std::transform(path.begin(), path.end(), path.begin(), tolower);
	size_t pos;
	while ((pos = path.find('\\')) != std::string::npos)
		path[pos] = '/';

	if (path[0] == '/')
		path = path.substr(1, path.length());

	return path;
}
