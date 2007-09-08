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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/exception.h"
#include "util/logger.h"

#include "vfssourcefactory.h"
#include "vfssourceprovider.h"

namespace FIFE {
	static Logger _log(LM_VFS);
	
	VFSSourceFactory::VFSSourceFactory() {}

	VFSSourceFactory::~VFSSourceFactory() {
		cleanup();
	}

	void VFSSourceFactory::cleanup() {
		type_providers::const_iterator end = m_providers.end();
		for (type_providers::iterator i = m_providers.begin(); i != end; ++i)
			delete *i;

		m_providers.clear();
	}

	void VFSSourceFactory::addProvider(VFSSourceProvider* provider) {
		m_providers.push_back(provider);
		FL_LOG(_log, LMsg("new provider: ") << provider->getName());
	}

	VFSSource* VFSSourceFactory::createSource(const std::string& file) const {
		if( m_usedfiles.count(file) ) {
			FL_WARN(_log, LMsg(file) << " is already used as VFS source");
			return 0;
		}

		type_providers::const_iterator end = m_providers.end();
		for (type_providers::const_iterator i = m_providers.begin(); i != end; ++i) {
			const VFSSourceProvider* provider = *i;
			if (!provider->isReadable(file))
				continue;

			try {
				VFSSource* source = provider->createSource(file);
				m_usedfiles.insert(file);
				return source;
			} catch(const Exception& ex) {
				FL_WARN(_log, LMsg(provider->getName()) << " thought it could load " << file << " but didn't succeed (" << ex.getMessage() << ")");
				continue;
			} catch(...) {
				FL_WARN(_log, LMsg(provider->getName()) << " thought it could load " << file << " but didn't succeed (unkown exception)");
				continue;
			}
		}

		FL_WARN(_log, LMsg("no provider for ") << file << " found");
		return 0;
	}
}
