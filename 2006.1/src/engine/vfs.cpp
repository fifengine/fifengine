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

#include "vfs.h"
#include "vfssource.h"
#include "log.h"
#include "rawdata.h"
#include "exception.h"

#include <algorithm>
#include <boost/functional.hpp>
#include <boost/regex.hpp>

namespace FIFE {

	VFS::VFS() : m_sources() {}

	VFS::~VFS() {
		cleanup();
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

	VFS::type_stringlist VFS::listFiles(const std::string& pathstr) const {
		VFS::type_stringlist list;
		type_sources::const_iterator end = m_sources.end();
		for (type_sources::const_iterator i = m_sources.begin(); i != end; ++i) {
			type_stringlist sourcelist = (*i)->listFiles(pathstr);
			list.insert(sourcelist.begin(), sourcelist.end());
		}

		return list;
	}

	VFS::type_stringlist VFS::listFiles(const std::string& path, const std::string& filterregex) const {
		VFS::type_stringlist list = listFiles(path);
		filterList(list, filterregex);
		return list;
	}

	VFS::type_stringlist VFS::listDirectories(const std::string& pathstr) const {
		VFS::type_stringlist list;
		type_sources::const_iterator end = m_sources.end();
		for (type_sources::const_iterator i = m_sources.begin(); i != end; ++i) {
			type_stringlist sourcelist = (*i)->listDirectories(pathstr);
			list.insert(sourcelist.begin(), sourcelist.end());
		}

		return list;
	}

	VFS::type_stringlist VFS::listDirectories(const std::string& path, const std::string& filterregex) const {
		VFS::type_stringlist list = listDirectories(path);
		filterList(list, filterregex);
		return list;
	}

	void VFS::filterList(type_stringlist& list, const std::string& fregex) const {
		boost::regex regex(fregex);
		type_stringlist::const_iterator end = list.end();
		for (type_stringlist::const_iterator i = list.begin(); i != end;) {
			boost::cmatch match;
			if (!boost::regex_match((*i).c_str(), match, regex))
				list.erase(i++);
			else
				++i;
		}

	}
}


