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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "renderbackends/sdl/renderbackendsdl.h"

#ifdef HAVE_OPENGL
#	include "renderbackends/opengl/renderbackendopengl.h"
#endif

#include "exception.h"
#include "log.h"

#include "renderbackend.h"
#include "rendermanager.h"

namespace FIFE {

	RenderManager::RenderManager() : m_backendsEnumerated(false) {
		enumerateBackends();
	}


	RenderManager::~RenderManager() {
	}

	void RenderManager::add(RenderBackend* backend) {
		Log("RenderManager") << "new backend: " << backend->getName();
		BackendManager<std::string, RenderBackend>::add(backend->getName(), backend);
	}

	void RenderManager::enumerateBackends() {
		if (m_backendsEnumerated)
			return;
#ifdef HAVE_OPENGL
		add(new RenderBackendOpenGL());
#endif
		add(new RenderBackendSDL());

		m_backendsEnumerated = true;
	};

	void RenderManager::select(const std::string& key) {
		Log("RenderManager") << "selecting backend: " << key;
		BackendManager<std::string, RenderBackend>::select(key);
	}
}


