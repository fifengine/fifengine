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

#ifndef FIFE_RAWDATASOURCE_H
#define FIFE_RAWDATASOURCE_H

// Standard C++ library includes

// Platform specific includes
#include "util/fifeint.h"

// 3rd party library includes

// FIFE includes

namespace FIFE {

	/** Abstract baseclass - provides data for RawData
	 *
	 * RawData uses RawDataSources to access the data - if you want to add support for a new archivetype this is one of
	 * the classes you need.
	 */
	class RawDataSource {

		public:
			RawDataSource();
			virtual ~RawDataSource();

			/** get the complete datasize */
			virtual unsigned int getSize() const = 0;

			/** read data from the source
			 *
			 * @param buffer the data will be written into buffer
			 * @param start the startindex inside the source
			 * @param length length bytes will be written into buffer
			 */
			virtual void readInto(uint8_t* buffer, unsigned int start, unsigned int length) = 0;

	};

}

#endif
