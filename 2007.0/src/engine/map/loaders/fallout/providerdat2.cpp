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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"

#include "dat2.h"
#include "providerdat2.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {
	VFSSource* ProviderDAT2::createSource(const std::string& file) const {
		return new DAT2(file);
	}

	bool ProviderDAT2::isReadable(const std::string& file) const {
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

} } } }
