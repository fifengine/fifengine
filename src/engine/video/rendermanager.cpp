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

#include "rendermanager.h"
#include "renderbackend.h"
#include "log.h"
#include "exception.h"

#include "renderbackends/sdl/renderbackendsdl.h"

#ifdef HAVE_OPENGL
#	include "renderbackends/opengl/renderbackendopengl.h"
#endif


namespace FIFE {

	RenderManager::RenderManager() : m_active_backend(0), m_backendsEnumerated(false) {
	}


	RenderManager::~RenderManager() {
		if (m_active_backend)
			m_active_backend->deinit();

		cleanup();
	}

	void RenderManager::cleanup() {
		type_backends::const_iterator end = m_backends.end();
		for (type_backends::iterator i = m_backends.begin(); i != end; ++i)
			delete i->second;

		m_backends.clear();
	}

	bool RenderManager::exists(const std::string& name) const {
		return m_backends.find(name) != m_backends.end();
	}

	void RenderManager::registerBackend(RenderBackend* backend) {
		const std::string& name = backend->getName();
		if (exists(name)) {
			Log("RenderManager") << "RenderBackend name " << name << " already exists!";
			return;
		}

		m_backends.insert(std::make_pair(name, backend));
		Log("RenderManager") << "new backend: " << name;
	}

	RenderBackend* RenderManager::getBackend(const std::string& name) const {
		type_backends::const_iterator i = m_backends.find(name);
		if (i == m_backends.end()) {
			throw NotFound(name + " not found");
		}

		return i->second;
	}

	RenderBackend* RenderManager::getCurrentBackend() const {
		return m_active_backend;
	}

	void RenderManager::selectBackend(const std::string& name) {
		enumerateBackends();
		Log("RenderManager") << "using backend: " << name;
		m_active_backend = getBackend(name);
		m_active_backend->init();
	}

	void RenderManager::enumerateBackends() {
		if( m_backendsEnumerated )
			return;
#ifdef HAVE_OPENGL
		registerBackend( new RenderBackendOpenGL() );
#endif
		registerBackend( new RenderBackendSDL() );

		m_backendsEnumerated = true;

	};
}


