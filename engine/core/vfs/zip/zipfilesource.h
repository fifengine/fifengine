/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#ifndef FIFE_VFS_ZIP_ZIPFILESOURCE_H
#define FIFE_VFS_ZIP_ZIPFILESOURCE_H

#include "vfs/raw/rawdatasource.h"

namespace FIFE {

	class VFS;
	class ZipFileSource : public RawDataSource {
		public:
			ZipFileSource(uint8_t* data, unsigned int datalen);
			virtual ~ZipFileSource();

			virtual unsigned int getSize() const;
			virtual void readInto(uint8_t* target, unsigned int start, unsigned int len);

		private:
			uint8_t* m_data;
			unsigned int m_datalen;

	};

}

#endif
