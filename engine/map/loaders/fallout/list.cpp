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
#include "util/exception.h"
#include "util/log.h"

#include "list.h"
#include "remove_all.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {

	void list::load() {
		RawDataPtr input = VFS::instance()->open(m_filename);

		std::string buffer;
		while (input->getLine(buffer)) {
			removeAll(buffer, " ");
			removeAll(buffer, "\r");

			m_list.push_back(buffer);
		}
	}

	const std::string& list::getProFile(size_t index) const {
		if (m_list.size() <= index) {
			Log("MFFalloutLST") << "size: " << m_list.size() << " requested: " << index;
			throw IndexOverflow("Index overflow in " + m_filename);
		}

		return *(m_list.begin() + index);
	}

} } } } //FIFE::map::loaders::fallout

