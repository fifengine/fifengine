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
#ifndef FIFEVFSSOURCEFACTORY_H
#define FIFEVFSSOURCEFACTORY_H

#include "singleton.h"
#include <vector>

namespace FIFE {
	class VFSSourceProvider;
	class VFSSource;

	/** creates VFSSources through VFSSourceProviders
	 *
	 */
	class VFSSourceFactory : public Singleton<VFSSourceFactory> {
		public:

			/** add new VFSSourceProvider
			 *
			 * NOTE: VFSSourceFactory assumes ownership over the given provider - so don't do anything with it after you call
			 * this function, espacily don't delete it!
			 * @param provider the new provider
			 */
			void addProvider(VFSSourceProvider* provider);

			/** try's to create a new VFSSource for the given file
			 *
			 * all currently known VFSSourceProviders are tried until one succeeds - if no provider succeeds 0 is returned
			 * @param file the archive-file
			 * @return the new VFSSource or 0 if no provider was succesfull
			 */
			VFSSource* createSource(const std::string& file) const;

		private:
			typedef std::vector<VFSSourceProvider*> type_providers;
			type_providers m_providers;

			void cleanup();

			SINGLEFRIEND(VFSSourceFactory);

	};

}

#endif
