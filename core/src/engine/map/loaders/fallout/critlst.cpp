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
#include "vfs/vfs.h"
#include "vfs/raw/rawdata.h"
#include "util/exception.h"
#include "log.h"

#include "critlst.h"
#include "remove_all.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {

	CritLST::CritLST(const std::string& path) : m_filename(path), m_filelist() {
		RawDataPtr input = VFS::instance()->open(m_filename);

		std::string buffer;
		bool firstLine = true;
		while (input->getLine(buffer)) {
			removeAll(buffer, " ");
			removeAll(buffer, "\r");

			int a = buffer.find(',', 0);
			if (a == -1) {
				if (firstLine) { // reserved entry
					m_filelist.push_back("");
					m_indexlist.push_back(0);
					m_speciallist.push_back(false);
					firstLine = false;
					continue;
				} else {
					Log("MFFalloutCritLST") << "line: " << buffer << " is invalid!";
					throw InvalidFormat("Line does not match: a, b [,c]");
				}
			}
			m_filelist.push_back(buffer.substr(0, a));

			int b = buffer.find(',', a+1);
			int c = (b == -1) ? buffer.size() : b;
			// FIXME: lexical_cast? Maybe change the whole thing to use streams
			size_t tmp = atoi( buffer.substr(a+1, c - a - 1).c_str());
			m_indexlist.push_back(tmp);

			// XXX Where does the m_speciallist get used?
			m_speciallist.push_back(b != -1);
		}
	}

	const std::string& CritLST::getProFile(size_t index) const {
		if (index < 0 || m_filelist.size() <= index) {
			Log("MFFalloutCritLST") << "size: " << m_filelist.size() << " requested: " << index;
			throw IndexOverflow("Index overflow in " + m_filename);
		}

		return m_filelist[index];
	}
	
	const size_t CritLST::getReIndex(size_t index) {
		if (index < 0 || m_indexlist.size() <= index) {
			Log("MFFalloutCritLST") << "size: " << m_filelist.size() << " requested: " << index;
			throw IndexOverflow("Index overflow in " + m_filename);
		}
		return m_indexlist[index];
	}

} } } } //FIFE::map::loaders::fallout

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
