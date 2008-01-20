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

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes
#include <boost/functional.hpp>
#include <boost/regex.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "util/exception.h"
#include "util/logger.h"

#include "vfs.h"
#include "vfssource.h"
#include "vfssourceprovider.h"

namespace FIFE {
	static Logger _log(LM_VFS);


	VFS::VFS() : m_sources() {}

	VFS::~VFS() {
		cleanup();
	}

	void VFS::cleanup() {
		type_sources sources = m_sources;
		type_sources::const_iterator end = sources.end();
		for (type_sources::iterator i = sources.begin(); i != end; ++i)
			delete *i;

		type_providers::const_iterator end2 = m_providers.end();
		for (type_providers::iterator j = m_providers.begin(); j != end2; ++j)
			delete *j;

		m_providers.clear();
	}

	void VFS::addProvider(VFSSourceProvider* provider) {
		m_providers.push_back(provider);
		FL_LOG(_log, LMsg("new provider: ") << provider->getName());
	}

	VFSSource* VFS::createSource(const std::string& path) const {
		if( m_usedfiles.count(path) ) {
			FL_WARN(_log, LMsg(path) << " is already used as VFS source");
			return 0;
		}

		type_providers::const_iterator end = m_providers.end();
		for (type_providers::const_iterator i = m_providers.begin(); i != end; ++i) {
			const VFSSourceProvider* provider = *i;
			if (!provider->isReadable(path))
				continue;

			try {
				VFSSource* source = provider->createSource(path);
				m_usedfiles.insert(path);
				return source;
			} catch(const Exception& ex) {
				FL_WARN(_log, LMsg(provider->getName()) << " thought it could load " << path << " but didn't succeed (" << ex.getMessage() << ")");
				continue;
			} catch(...) {
				FL_WARN(_log, LMsg(provider->getName()) << " thought it could load " << path << " but didn't succeed (unkown exception)");
				continue;
			}
		}

		FL_WARN(_log, LMsg("no provider for ") << path << " found");
		return 0;
	}

	void VFS::addNewSource(const std::string& path) {
		VFSSource* source = createSource(path);
		if (source) {
			addSource(source);
		} else {
			FL_WARN(_log, LMsg("Failed to add new VFS source: ") << path);
		}
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
		std::string lowerpath = lower(file);
		type_sources::const_iterator i = std::find_if(m_sources.begin(), m_sources.end(),
		                                 boost::bind2nd(boost::mem_fun(&VFSSource::fileExists), lowerpath));
		if (i == m_sources.end()) {
			FL_WARN(_log, LMsg("no source for ") << lowerpath << " found");
			return 0;
		}

		return *i;
	}

	bool VFS::exists(const std::string& file) const {
		return getSourceForFile(lower(file));
	}

	RawData* VFS::open(const std::string& path) {
		std::string lowerpath = lower(path);
		FL_DBG(_log, LMsg("Opening: ") << lowerpath);

		VFSSource* source = getSourceForFile(lowerpath);
		if (!source)
			throw NotFound(path);

		return source->open(lowerpath);
	}

	std::string VFS::lower(const std::string& str) const {
		std::string result;
		result.resize(str.size());
		std::transform(str.begin(), str.end(), result.begin(), tolower);
		return result;
	}

	std::vector<std::string> VFS::listFiles(const std::string& pathstr) const {
		std::string lowerpath = lower(pathstr);
		std::vector<std::string> list;
		type_sources::const_iterator end = m_sources.end();
		for (type_sources::const_iterator i = m_sources.begin(); i != end; ++i) {
			std::vector<std::string> sourcelist = (*i)->listFiles(lowerpath);
			list.insert(list.begin(), sourcelist.begin(), sourcelist.end());
		}

		return list;
	}

	std::vector<std::string> VFS::listFiles(const std::string& path, const std::string& filterregex) const {
		std::string lowerpath = lower(path);
		std::vector<std::string> list = listFiles(lowerpath);
		filterList(list, filterregex);
		return list;
	}

	std::vector<std::string> VFS::listDirectories(const std::string& pathstr) const {
		std::string lowerpath = lower(pathstr);
		std::vector<std::string> list;
		type_sources::const_iterator end = m_sources.end();
		for (type_sources::const_iterator i = m_sources.begin(); i != end; ++i) {
			std::vector<std::string> sourcelist = (*i)->listDirectories(lowerpath);
			list.insert(list.begin(), sourcelist.begin(), sourcelist.end());
		}

		return list;
	}

	std::vector<std::string> VFS::listDirectories(const std::string& path, const std::string& filterregex) const {
		std::vector<std::string> list = listDirectories(lower(path));
		filterList(list, filterregex);
		return list;
	}

	void VFS::filterList(std::vector<std::string>& list, const std::string& fregex) const {
		boost::regex regex(fregex);
		std::vector<std::string>::iterator end = list.end();
		for (std::vector<std::string>::iterator i = list.begin(); i != end;) {
			boost::cmatch match;
			if (!boost::regex_match((*i).c_str(), match, regex))
				list.erase(i++);
			else
				++i;
		}

	}
}
