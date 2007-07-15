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

#ifndef FIFE_MAP_LOADERS_FALLOUT_LIST_H
#define FIFE_MAP_LOADERS_FALLOUT_LIST_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE { namespace map { namespace loaders { namespace fallout {
	/** Class containing information about a list as it is used in the Fallout data files
	 *
	 */
	class list {
		public:
			/** Constructor
			 * This will load the list file.
			 *
			 * @param path The path where the list file is located.
			 * @throw CannotOpenFile 
			 */
			list(const std::string& path) : m_filename(path), m_list() { load(); }

			/** Get entry at index
			 * @param index Lookup index in the file counting from 0 inclusive
			 * @return The string at that index.
			 * @throw IndexOverflow
			 */
			const std::string& getProFile(size_t index) const;

		private:
			std::string m_filename;

			typedef std::vector<std::string> type_list;
			type_list m_list;

			void load();
	};

} } } } //FIFE

#endif

