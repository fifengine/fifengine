/***************************************************************************
 *   Copyright (C) 2005 by the FIFE Team                                   *
 *   fife@developer1.cvsdude.com                                           *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "mffalloutproviderdat2.h"
#include "mffalloutdat2.h"
#include "vfs.h"
#include "rawdata.h"

namespace FIFE {

	MFFalloutProviderDAT2::MFFalloutProviderDAT2() : VFSSourceProvider("Fallout2-dat") {
	}

	MFFalloutProviderDAT2::~MFFalloutProviderDAT2() {
	}

	VFSSource* MFFalloutProviderDAT2::createSource(const std::string& file) const {
		return new MFFalloutDAT2(file);
	}

	bool MFFalloutProviderDAT2::isReadable(const std::string& file) const {
		if (file.find(".dat") == std::string::npos)
			return false;

		VFS* vfs = VFS::instance();
		if (!vfs->exists(file))
			return false;

		RawDataPtr f = vfs->open(file);
		f->setIndex(f->getDataLength() - 4);
		uint32_t archivesize = f->read32Little();
		return archivesize == f->getDataLength();
	}

}
