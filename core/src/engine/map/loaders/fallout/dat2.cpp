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
#include <boost/bind.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "util/exception.h"
#include "log.h"

#include "dat2.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {

	DAT2::DAT2(const std::string& file) 
		: m_datpath(file), m_data(VFS::instance()->open(file)), m_filelist() {

		Log("MFFalloutDAT2") 
			<< "loading: " << file 
			<< " filesize: " << m_data->getDataLength();

		m_data->setIndex(m_data->getDataLength() - 8);
		uint32_t fileListLength = m_data->read32Little();
		uint32_t archiveSize = m_data->read32Little();

		Log("MFFalloutDAT2") 
			<< "FileListLength: " << fileListLength 
			<< " ArchiveSize: " << archiveSize;

		if (archiveSize != m_data->getDataLength())
			throw InvalidFormat("size mismatch");

		m_data->setIndex( archiveSize - fileListLength - 8);
		m_filecount = m_data->read32Little();
		m_currentIndex = m_data->getCurrentIndex();

		Log("MFFalloutDAT2") << "FileCount: " << m_filecount;

		// Do not read the complete file list at startup.
		// Instead read a chunk each frame.
		m_timer.setInterval(0);
		m_timer.setCallback( boost::bind( &DAT2::readFileEntry, this) );
		m_timer.start();
	}

	void DAT2::readFileEntry() const {
		assert( m_filecount != 0);

		// Load more items per call,
		// otherwise it takes _ages_ until everything is in.
		unsigned int load_per_cycle = 50;
		if( load_per_cycle > m_filecount )
			load_per_cycle = m_filecount;
		m_filecount -= load_per_cycle;

		// Save the old index in an exception save way.
		IndexSaver isaver(m_data);

		// Move index to file list and read the entries.
		m_data->setIndex(m_currentIndex);
		RawDataDAT2::s_info info;
		while( load_per_cycle-- ) {
			uint32_t namelen = m_data->read32Little();
			info.name = fixPath(m_data->readString(namelen));
	
			info.type = m_data->read8();
			info.unpackedLength = m_data->read32Little();
			info.packedLength = m_data->read32Little();
			info.offset = m_data->read32Little();
	
			m_filelist.insert(std::make_pair(info.name, info));
		}
		m_currentIndex = m_data->getCurrentIndex();

		// Finally log on completion and stop the timer.
		if( m_filecount == 0 ) {
			Log("MFFalloutDAT2")
				<< "All file entries in '" << m_datpath << "' loaded.";
			m_timer.stop();
		}
	}

	RawData* DAT2::open(const std::string& file) const {
		const RawDataDAT2::s_info& info = getInfo(file);
		return new RawData(new RawDataDAT2(m_datpath, info));
	}

	bool DAT2::fileExists(const std::string& name) const {
		return findFileEntry(name) != m_filelist.end();
	}

	const RawDataDAT2::s_info& DAT2::getInfo(const std::string& name) const {
		type_filelist::const_iterator i = findFileEntry(name);
		if (i == m_filelist.end()) {
			throw NotFound(name);
		}
		return i->second;
	}

	DAT2::type_filelist::const_iterator DAT2::findFileEntry(const std::string& path) const {

		// Either the normalization is bogus, or we have to do
		// it here, too. Otherwise we can't load the files returned
		// by listFiles.

		std::string name = path;

		// Normalize the path
		if (name.find("./") == 0) {
			name.erase(0, 2);
		}

		type_filelist::const_iterator i = m_filelist.find(name);

		// We might have another chance to find the file
		// if the number of file entries not zero.
		if ( m_filecount && i == m_filelist.end()) {
			Log("MFFalloutDAT2")
				<< "Missing '" << name
				<< "' in partially(" << m_filecount <<") loaded "<< m_datpath;
			while( m_filecount && i == m_filelist.end()) {
				readFileEntry();
				i = m_filelist.find(name);
			}
		}
		return i;
	}


	VFS::type_stringlist DAT2::listFiles(const std::string& pathstr) const {
		return list(pathstr, false);
	}

	VFS::type_stringlist DAT2::listDirectories(const std::string& pathstr) const {
		return list(pathstr, true);
	}

	VFS::type_stringlist DAT2::list(const std::string& pathstr, bool dirs) const {
		VFS::type_stringlist list;
		std::string path = pathstr;

		// Force loading the complete file entries
		// This is a costly operation... right after startup.
		// Later this should do nothing.
		while( m_filecount ) {
			readFileEntry();
		}

		// Normalize the path
		if (path.find("./") == 0) {
			path.erase(0, 2);
		}
		
		int lastIndex = path.size() - 1;
		if ((lastIndex != -1) && path[lastIndex] != '/') {
			path += '/';
		}

		type_filelist::const_iterator end = m_filelist.end();
		for (type_filelist::const_iterator i = m_filelist.begin(); i != end; ++i) {
			const std::string& file = i->first;
			if (file.find(path) == 0) {
				std::string cleanedfile = file.substr(path.size(), file.size()); // strip the pathstr
				bool isdir = cleanedfile.find('/') != std::string::npos; // if we still have a / it's a subdir

				if (isdir) {
					cleanedfile = cleanedfile.substr(0, cleanedfile.find('/'));
					if (cleanedfile.find('/') != cleanedfile.rfind('/')) {
						// check if this is a direct subdir
						continue;
					}
				}

				if (isdir == dirs) {
					list.insert(cleanedfile);
				}
			}
		}

		return list;
	}
} } } } // ~FIFE::map::loaders::fallout
