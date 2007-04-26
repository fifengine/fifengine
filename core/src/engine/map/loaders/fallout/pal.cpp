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
#include "debugutils.h"
#include "util/exception.h"

#include "pal.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {

	PAL::PAL(const std::string& filename) {
		RawDataPtr input = VFS::instance()->open(filename);

		if (input->getDataLength() < PALSIZE) {
			Log("mlfallout") << "filesize: " << input->getDataLength() 
			                 << " wanted: " << PALSIZE;
			throw InvalidFormat("size mismatch in " + filename);
		}
		input->readInto(m_palette, PALSIZE);
	}

	uint8_t PAL::get(size_t index) const {
		if (index >= PALSIZE) {
			throw IndexOverflow( __FUNCTION__ );
		}

		return m_palette[index];
	}

	uint32_t PAL::operator[](uint8_t index) const {
		return getRGB(index);
	}

	uint32_t PAL::getRGB(uint8_t index) const {
		if (static_cast<unsigned int>(index * 3 + 2) >= PALSIZE) {
			throw IndexOverflow( __FUNCTION__ );
		}

		size_t sindex = index * 3;
		return (m_palette[sindex] << 24) | (m_palette[sindex + 1] << 16) | 
		       (m_palette[sindex + 2] << 8) | 0xff;
	}

	uint8_t PAL::getRed(uint8_t index) const {
		return get(index * 3);
	}

	uint8_t PAL::getGreen(uint8_t index) const {
		return get(index * 3 + 1);
	}

	uint8_t PAL::getBlue(uint8_t index) const {
		return get(index * 3 + 2);
	}

} } } } //FIFE::map::loaders::fallout
