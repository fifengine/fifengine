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

#include "mffalloutcritlst.h"
#include "exception.h"
#include "vfs.h"
#include "rawdata.h"
#include "log.h"

namespace FIFE {

	MFFalloutCritLST::MFFalloutCritLST(const std::string& path) : m_filename(path), m_filelist() {
		load();
	}

	MFFalloutCritLST::~MFFalloutCritLST() {}

	void MFFalloutCritLST::load() {
		RawDataPtr input = VFS::instance()->open(m_filename);

		std::string buffer;
		bool firstLine = true;
		while (input->getLine(buffer)) {
			buffer = cleanupString(buffer, " ");
			buffer = cleanupString(buffer, "\r");

			int a = buffer.find(',', 0);
			int b = buffer.find(',', a+1);
			if ((a == -1)) {
				if (firstLine) { // reserved entry
					m_filelist.push_back("");
					m_indexlist.push_back(0);
					m_speciallist.push_back(false);
					firstLine = false;
					continue;
				}
				else {
					Log("MFFalloutCritLST") << "line: " << buffer << " is invalid!";
					throw InvalidFormat("Line does not match: a, b [,c]");
				}
			}
			m_filelist.push_back(buffer.substr(0, a));

			int c = (b == -1) ? buffer.size() : b;
			// std::cout << a << " , " << b << " , " << c << std::endl;
			size_t tmp = atoi( buffer.substr(a+1, c - a - 1).c_str());
			m_indexlist.push_back(tmp);

			if (b == -1)
				m_speciallist.push_back(false);
			else {
				m_speciallist.push_back(true);
				//std::cout << "special bit set" << std::endl;
			}

		}
	}

	const std::string& MFFalloutCritLST::getProFile(size_t index) const {
		if (m_filelist.size() <= index) {
			Log("MFFalloutCritLST") << "size: " << m_filelist.size() << " requested: " << index;
			throw IndexOverflow("Index overflow in " + m_filename);
		}

		return *(m_filelist.begin() + index);
	}
	const size_t MFFalloutCritLST::getReIndex(size_t index) {
		if (m_indexlist.size() <= index) {
			Log("MFFalloutCritLST") << "size: " << m_filelist.size() << " requested: " << index;
			throw IndexOverflow("Index overflow in " + m_filename);
		}
		return m_indexlist[index];
	}

	std::string MFFalloutCritLST::cleanupString(std::string str, const std::string& c) const {
		size_t pos = str.find(c);
		while (pos != std::string::npos) {
			str.erase(pos);
			pos = str.find(c);
		}

		return str;
	}

}//FIFE

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
