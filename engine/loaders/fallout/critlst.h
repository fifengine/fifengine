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

#ifndef FIFE_MAP_LOADERS_FALLOUT_CRITLST_H
#define FIFE_MAP_LOADERS_FALLOUT_CRITLST_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes

namespace FIFE { namespace map { namespace loaders { namespace fallout {
	/** Class containing information about a list of critters files
	 *
	 * These files contain entries:
	 * @code
	 * filename index special-index
	 * @endcode
	 * 
	 * This class reads these files in and makes index and special-index
	 * look ups possible.
	 * 
	 * Best to look up the Fallout Format specs. if you need more info.
	 */
	class CritLST {
		public:
			/** Constructor
			 * This will load the list file.
			 *
			 * @param path The path where the list file is located.
			 * @throw CannotOpenFile 
			 */
			CritLST(const std::string& path);

			/** Get the filename for the index.
			 */
			const std::string& getProFile(size_t index) const;

			/** Get the special-index for an index
			 */
			const size_t getReIndex(size_t index);

		private:
			std::string m_filename;

			typedef std::vector<std::string> type_list;
			type_list m_filelist;

			typedef std::vector<size_t> index_list;
			index_list m_indexlist;

			typedef std::vector<bool> special_list;
			special_list m_speciallist;
	};

} } } } // FIFE::map::loaders::fallout

#endif

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
