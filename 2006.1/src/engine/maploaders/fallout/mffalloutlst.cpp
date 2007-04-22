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

#include "mffalloutlst.h"
#include "exception.h"
#include "vfs.h"
#include "rawdata.h"
#include "log.h"

namespace FIFE {

	MFFalloutLST::MFFalloutLST(const std::string& path) : m_filename(path), m_list() {
		load();
	}

	MFFalloutLST::~MFFalloutLST() {}

	void MFFalloutLST::load() {
		RawDataPtr input = VFS::instance()->open(m_filename);

		std::string buffer;
		while (input->getLine(buffer)) {
			buffer = cleanupString(buffer, " ");
			buffer = cleanupString(buffer, "\r");

			m_list.push_back(buffer);
		}
	}

	const std::string& MFFalloutLST::getProFile(size_t index) const {
		if (m_list.size() <= index) {
			Log("MFFalloutLST") << "size: " << m_list.size() << " requested: " << index;
			throw IndexOverflow("Index overflow in " + m_filename);
		}

		return *(m_list.begin() + index);
	}

	std::string MFFalloutLST::cleanupString(std::string str, const std::string& c) const {
		size_t pos = str.find(c);
		while (pos != std::string::npos) {
			str.erase(pos);
			pos = str.find(c);
		}

		return str;
	}

}//FIFE

