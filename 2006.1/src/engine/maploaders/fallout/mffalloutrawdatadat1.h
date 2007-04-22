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

#ifndef FIFEMFFALLOUTRAWDATADAT1_H
#define FIFEMFFALLOUTRAWDATADAT1_H

#include "rawdatamemsource.h"
#include "vfs.h"

#include <string>


namespace FIFE {

	/** A subclass of RawDataMemSource, that fills itself with a FALLOUT1 .DAT file entry
	 * @see MFFalloutDAT1
	 */
	class MFFalloutRawDataDAT1 : public RawDataMemSource {
		public:
			/** The needed information for the extraction.
			 */
			struct s_info {
				std::string name;
				uint32_t packedLength;
				uint32_t unpackedLength;
				uint32_t offset;
				uint32_t type;
			};

			/** Constructor
			 * @param datfile The .DAT archive - e.g. master.DAT
			 * @param info The .DAT file entry, as retrieved by MFFalloutDAT1
			 */
			MFFalloutRawDataDAT1(const std::string& datfile, const s_info& info);

			/** Destructor
			 */
			virtual ~MFFalloutRawDataDAT1();


		private:
	};

}

#endif
