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

#ifndef FIFERENDERMANAGER_H
#define FIFERENDERMANAGER_H

#include "singleton.h"
#include <map>
#include <string>

namespace FIFE {

	class RenderBackend;

	/** RenderBackend-manager
	 *
	 * Knows all available RenderBackends,
	 */
	class RenderManager : public DynamicSingleton<RenderManager> {
		public:
			RenderManager();
			virtual ~RenderManager();

			/** Register a new RenderBackend
			 *
			 * @param backend the new backend
			 */
			void registerBackend(RenderBackend* backend);

			/** Returns the renderbackend named "name"
			 *
			 * @param name the name of the renderbackend
			 * @throws NotFound if no backend with the given name exists
			 */
			RenderBackend* getBackend(const std::string& name) const;

			/** Test if a backend with the given name exists
			 *	@return true if it exists, otherwise false
			 */
			bool exists(const std::string& name) const;

			/** Select the active renderbackend
			 *
			 * this will initalize the backend
			 * @param name the name of the backend
			 */
			void selectBackend(const std::string& name);

			/** Get the active backend
			 *
			 * @return the current backend, or 0 if none has been selected
			 * @see selectBackend
			 */
			RenderBackend* getCurrentBackend() const;

		private:
			typedef std::map<std::string, RenderBackend*> type_backends;
			type_backends m_backends;

			RenderBackend* m_active_backend;

			bool m_backendsEnumerated;
			void enumerateBackends();
			void cleanup();
	};

}

#endif
