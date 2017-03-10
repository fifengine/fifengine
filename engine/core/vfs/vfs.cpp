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

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes
#include <boost/functional.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"

#include "vfs.h"
#include "vfssource.h"
#include "vfssourceprovider.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
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
		provider->setVFS(this);
		m_providers.push_back(provider);
		FL_LOG(_log, LMsg("new provider: ") << provider->getName());
	}

	VFSSource* VFS::createSource(const std::string& path) {

		if (hasSource(path)) {
			FL_WARN(_log, LMsg(path) << " is already used as VFS source");
			return 0;
		}

		type_providers::const_iterator end = m_providers.end();
		for (type_providers::const_iterator i = m_providers.begin(); i != end; ++i) {
			VFSSourceProvider* provider = *i;
			if (!provider->isReadable(path))
				continue;

			try {
				VFSSource* source = provider->createSource(path);
				return source;
			} catch (const Exception& ex) {
				FL_WARN(_log, LMsg(provider->getName()) << " thought it could load " << path << " but didn't succeed (" << ex.what() << ")");
				continue;
			} catch (...) {
				FL_WARN(_log, LMsg(provider->getName()) << " thought it could load " << path << " but didn't succeed (unknown exception)");
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

	void VFS::removeSource(const std::string& path) {
		type_providers::iterator end = m_providers.end();
		for (type_providers::iterator i = m_providers.begin(); i != end; ++i) {
			VFSSourceProvider* provider = *i;
			if (provider->hasSource(path)) {
				VFSSource* source = provider->getSource(path);
				type_sources::iterator i = std::find(m_sources.begin(), m_sources.end(), source);
				if (i == m_sources.end()) {
					removeSource(*i);
					return;
				}
			}
		}
	}

	VFSSource* VFS::getSourceForFile(const std::string& file) const {
		type_sources::const_iterator i = std::find_if(m_sources.begin(), m_sources.end(),
										 boost::bind2nd(boost::mem_fun(&VFSSource::fileExists), file));
		if (i == m_sources.end()) {
			FL_WARN(_log, LMsg("no source for ") << file << " found");
			return 0;
		}

		return *i;
	}

	bool VFS::exists(const std::string& file) const {
		return getSourceForFile(file) != 0;
	}

	bool VFS::isDirectory(const std::string& path) const {
		std::vector<std::string> tokens;
		// Add a slash in case there isn't one in the string
		const std::string newpath = path + "/";
		boost::algorithm::split(tokens, newpath, boost::algorithm::is_any_of("/"));

		std::string currentpath = "/";
		std::vector<std::string>::const_iterator token=tokens.begin();
		while (token != tokens.end()) {
			if (*token != "") {
				if (*token != "." && *token != ".." && listDirectories(currentpath, *token).size() == 0) {
					return false;
				} else {
					currentpath += *token + "/";
				}
			}
			++token;
		}

		return true;
	}

	RawData* VFS::open(const std::string& path) {
		FL_DBG(_log, LMsg("Opening: ") << path);

		VFSSource* source = getSourceForFile(path);
		if (!source)
			throw NotFound(path);

		return source->open(path);
	}

	std::set<std::string> VFS::listFiles(const std::string& pathstr) const {
		std::set<std::string> list;
		type_sources::const_iterator end = m_sources.end();
		for (type_sources::const_iterator i = m_sources.begin(); i != end; ++i) {
			std::set<std::string> sourcelist = (*i)->listFiles(pathstr);
			list.insert(sourcelist.begin(), sourcelist.end());
		}

		return list;
	}

	std::set<std::string> VFS::listFiles(const std::string& path, const std::string& filterregex) const {
		std::set<std::string> list = listFiles(path);
		return filterList(list, filterregex);
	}

	std::set<std::string> VFS::listDirectories(const std::string& pathstr) const {
		std::set<std::string> list;
		type_sources::const_iterator end = m_sources.end();
		for (type_sources::const_iterator i = m_sources.begin(); i != end; ++i) {
			std::set<std::string> sourcelist = (*i)->listDirectories(pathstr);
			list.insert(sourcelist.begin(), sourcelist.end());
		}

		return list;
	}

	std::set<std::string> VFS::listDirectories(const std::string& path, const std::string& filterregex) const {
		std::set<std::string> list = listDirectories(path);
		return filterList(list, filterregex);
	}

	std::set<std::string> VFS::filterList(const std::set<std::string>& list, const std::string& fregex) const {
		std::set<std::string> results;
		boost::regex regex(fregex);
		std::set<std::string>::const_iterator end = list.end();
		for (std::set<std::string>::const_iterator i = list.begin(); i != end;) {
			boost::cmatch match;
			if (boost::regex_match((*i).c_str(), match, regex)) {
				results.insert(*i);
			}
			++i;
		}
		return results;
	}

	bool VFS::hasSource(const std::string& path) const {
		type_providers::const_iterator end = m_providers.end();
		for (type_providers::const_iterator i = m_providers.begin(); i != end; ++i) {
			const VFSSourceProvider* provider = *i;
			if (provider->hasSource(path)) {
				const VFSSource* source = provider->getSource(path);
				type_sources::const_iterator i = std::find(m_sources.begin(), m_sources.end(), source);
				if (i == m_sources.end())
					return false;
				return true;
			}
		}
		return false;
	}
}
