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
#include "vfssourcefactory.h"
#include "vfssourceprovider.h"
#include "log.h"
#include "exception.h"

namespace FIFE {

	VFSSourceFactory::VFSSourceFactory() {}

	VFSSourceFactory::~VFSSourceFactory() {
		cleanup();
	}

	VFSSourceFactory::VFSSourceFactory(const VFSSourceFactory&) {}

	VFSSourceFactory& VFSSourceFactory::operator=(const VFSSourceFactory&) {
		return *this;
	}

	void VFSSourceFactory::cleanup() {
		type_providers::const_iterator end = m_providers.end();
		for (type_providers::iterator i = m_providers.begin(); i != end; ++i)
			delete *i;

		m_providers.clear();
	}

	void VFSSourceFactory::addProvider(VFSSourceProvider* provider) {
		m_providers.push_back(provider);
		Log("VFSSourceFactory") << "new provider: " << provider->getName();
	}

	VFSSource* VFSSourceFactory::createSource(const std::string& file) const {
		type_providers::const_iterator end = m_providers.end();
		for (type_providers::const_iterator i = m_providers.begin(); i != end; ++i) {
			const VFSSourceProvider* provider = *i;
			if (!provider->isReadable(file))
				continue;

			try {
				VFSSource* source = provider->createSource(file);
				return source;
			} catch(const Exception& ex) {
				Log("VFSSourceFactory") << provider->getName() << " thought it could load " << file << " but didn't succeed (" << ex.getMessage() << ")";
				continue;
			} catch(...) {
				Log("VFSSourceFactory") << provider->getName() << " thought it could load " << file << " but didn't succeed (unkown exception)";
				continue;
			}
		}

		Log("VFSSourceFactory") << "no provider for " << file << " found";
		return 0;
	}
}
