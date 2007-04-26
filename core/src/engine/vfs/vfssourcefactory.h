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

#ifndef FIFE_VFS_VFSSOURCEFACTORY_H
#define FIFE_VFS_VFSSOURCEFACTORY_H

// Standard C++ library includes
#include <vector>
#include <set>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/singleton.h"

namespace FIFE {
	class VFSSourceProvider;
	class VFSSource;

	/** creates VFSSources through VFSSourceProviders
	 *
	 */
	class VFSSourceFactory : public DynamicSingleton<VFSSourceFactory> {
		public:
			VFSSourceFactory();
			virtual ~VFSSourceFactory();

			/** add new VFSSourceProvider
			 *
			 * @NOTE VFSSourceFactory assumes ownership over the given provider - so don't do anything with it
			 * after you call this function, especialy don't delete it!
			 * @param provider the new provider
			 */
			void addProvider(VFSSourceProvider* provider);

			/** try's to create a new VFSSource for the given file
			 *
			 * all currently known VFSSourceProviders are tried until one succeeds - if no provider succeeds 0 is returned
			 * @param file the archive-file
			 * @return the new VFSSource or 0 if no provider was succesfull or the file was already used as source
			 */
			VFSSource* createSource(const std::string& file) const;

		private:
			typedef std::set<std::string> type_usedfiles;
			typedef std::vector<VFSSourceProvider*> type_providers;
			type_providers m_providers;
			mutable type_usedfiles m_usedfiles;

			void cleanup();

	};

}

#endif
