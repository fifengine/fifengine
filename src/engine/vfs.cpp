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

#include "vfs.h"
#include "vfssource.h"
#include "log.h"
#include "rawdata.h"
#include "exception.h"
#include <algorithm>
#include <boost/functional.hpp>

namespace FIFE {

	VFS::VFS() : m_sources() {}

	VFS::~VFS() {
		cleanup();
	}

	VFS::VFS(const VFS&) : m_sources() {}

	VFS& VFS::operator=(const VFS&) {
		return *this;
	}


	void VFS::cleanup() {
		type_sources sources = m_sources;
		type_sources::const_iterator end = sources.end();
		for (type_sources::iterator i = sources.begin(); i != end; ++i)
			delete *i;
	}

	void VFS::addSource(VFSSource* source) {
		m_sources.push_back(source);
	}

	void VFS::removeSource(VFSSource* source) {
		type_sources::iterator i = std::find(m_sources.begin(), m_sources.end(), source);
		if (i != m_sources.end())
			m_sources.erase(i);
	}

	VFSSource* VFS::getSourceForFile(const std::string& file) const {
		type_sources::const_iterator i = std::find_if(m_sources.begin(), m_sources.end(),
		                                 boost::bind2nd(boost::mem_fun(&VFSSource::fileExists), file));
		if (i == m_sources.end()) {
			Log("VFS") << "no source for " << file << " found";
			return 0;
		}

		return *i;
	}

	bool VFS::exists(const std::string& file) const {
		return getSourceForFile(lower(file));
	}

	RawDataPtr VFS::open(const std::string& path) {
		std::string lowerpath = lower(path);
		VFSSource* source = getSourceForFile(lowerpath);
		if (!source)
			throw NotFound(path);

		RawDataPtr data(source->open(lowerpath));
		return data;
	}

	std::string VFS::lower(const std::string& str) const {
		std::string result;
		result.resize(str.size());
		std::transform(str.begin(), str.end(), result.begin(), tolower);
		return result;
	}
}
