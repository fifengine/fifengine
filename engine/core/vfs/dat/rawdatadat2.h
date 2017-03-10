/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_MAP_LOADERS_FALLOUT_RAWDATADAT2_H
#define FIFE_MAP_LOADERS_FALLOUT_RAWDATADAT2_H

// Standard C++ library includes
#include <string>

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdatamemsource.h"

#include "rawdatadat2.h"

namespace FIFE {

	/** A subclass of RawDataMemSource, that fills itself with a FALLOUT1 .DAT file entry
	 * @see MFFalloutDAT2
	 */
	class RawDataDAT2 : public RawDataMemSource {
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
			 * @param vfs The vfs to open with.
			 * @param datfile The .DAT archive - e.g. master.DAT
			 * @param info The .DAT file entry, as retrieved by MFFalloutDAT2
			 */
			RawDataDAT2(VFS* vfs, const std::string& datfile, const s_info& info);
	};
}
#endif
