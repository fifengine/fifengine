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

#ifndef IANFALLOUTLST_H
#define IANFALLOUTLST_H

#include <string>
#include <vector>

namespace FIFE {
	/** Class containing information about a list as it is used in the Fallout data files
	 *
	 */
	class MFFalloutLST {
		public:
			/** Constructor
			 * This will load the list file.
			 *
			 * @param path The path where the list file is located.
			 * @throw CannotOpenFile 
			 */
			MFFalloutLST(const std::string& path);

			/** Destructor
			 */
			~MFFalloutLST();

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
			std::string cleanupString(std::string str, const std::string& c) const;

	};

}//FIFE

#endif

