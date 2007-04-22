/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *    fife-public@lists.sourceforge.net                                    *
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

#ifndef FIFEVFSSOURCE_H
#define FIFEVFSSOURCE_H

#include <string>

namespace FIFE {

	class RawData;
	/** VFSSource abstract baseclass
	 *
	 * VFSSources are used to provide transparent access to diffrent archives/filesystems etc.
	 * @see VFS
	 * @see VFSSourceProvider
	 */
	class VFSSource {
		public:
			VFSSource();
			virtual ~VFSSource();

			/** check if the given file exists
			 *
			 * @param file filename to check
			 * @return true if it exists, false otherwise
			 */
			virtual bool fileExists(const std::string& file) const = 0;

			/** open a file inside this source
			 *
			 * @param file the file to open
			 * @return a new RawData*
			 */
			virtual RawData* open(const std::string& file) const = 0;

		protected:
			std::string fixPath(std::string path) const;

	};

}

#endif
