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

#ifndef FIFEVFS_H
#define FIFEVFS_H

#include "singleton.h"
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

namespace FIFE {

	class RawData;
	typedef ::boost::shared_ptr<RawData> RawDataPtr;

	class VFSSource;

	/** the main VFS (virtual file system) class
	 *
	 * The VFS is intended to provide transparent and portable access to files.
	 */
	class VFS : public Singleton<VFS> {
		public:
			void cleanup();

			/** Add a new VFSSource */
			void addSource(VFSSource* source);

			/** remove a VFSSource */
			void removeSource(VFSSource* source);

			/** Check if the given file exists
			 *
			 * @param file the filename
			 * @return true if it exists, false if not
			 */
			bool exists(const std::string& file) const;

			/** Open a file
			 *
			 * @param path the file to open
			 * @return the opened file (note: dont delete this - it's referenced count and will be deleted automatically)
			 * @throws NotFound if the file cannot be found
			 */
			RawDataPtr open(const std::string& path);

		private:
			typedef std::vector<VFSSource*> type_sources;
			type_sources m_sources;

			std::string lower(const std::string&) const;
			VFSSource* getSourceForFile(const std::string& file) const;

			SINGLEFRIEND(VFS);

	};

}

#endif
