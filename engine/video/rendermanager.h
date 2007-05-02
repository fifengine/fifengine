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

#ifndef FIFE_VIDEO_RENDERMANAGER_H
#define FIFE_VIDEO_RENDERMANAGER_H

// Standard C++ library includes
#include <map>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/backendmanager.h"
#include "util/singleton.h"

#include "renderbackend.h"

namespace FIFE {

	/** RenderBackend Manager.
	 *
	 * Knows all available RenderBackends.
	 */
	class RenderManager : public DynamicSingleton<RenderManager>, public BackendManager<std::string, RenderBackend> {
		public:
			/** Constructor.
			 */
			RenderManager();
			/** Virtual destructor.
			 */
			virtual ~RenderManager();

			/** Adds a new backend
			 */
			virtual void add(RenderBackend* backend);

			/** Selects the backend identified by key
			 */
			virtual void select(const std::string& key);

		private:
			// Indicates if the available renderbackends have been registered or not.
			bool m_backendsEnumerated;
			/** Registers all the available renderbackends.
			 */
			void enumerateBackends();
			/** Deletes all the available renderbackends.
			 */
			void cleanup();
	};

	class CRenderBackend {
		public:
			RenderBackend* operator->() const {
				return RenderManager::instance()->current();
			};
	};

}

#endif
