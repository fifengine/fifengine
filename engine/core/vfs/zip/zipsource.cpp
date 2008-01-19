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
#include <algorithm>
#include <list>

// 3rd party library includes
#include "unzip.h"

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/exception.h"
#include "vfs/raw/rawdata.h"

#include "zipsource.h"
#include "zipdata.h"

namespace FIFE {
	typedef std::list<std::string> file_list_t;
	typedef file_list_t::iterator file_list_iter_t;

	struct ZipSource::zip_data_t {
		std::string zipName;
		unzFile file;
		file_list_t fileList;
	};

	static inline std::string unzip_error_tostring(int error) {
		switch (error) {
			case UNZ_END_OF_LIST_OF_FILE:
				return "End of List of File";
			case UNZ_ERRNO:
				return "Errno";
			case UNZ_EOF:
				return "End of File";
			case UNZ_PARAMERROR:
				return "Parameter Error";
			case UNZ_BADZIPFILE:
				return "Bad Zip File";
			case UNZ_INTERNALERROR:
				return "Internal Error";
			case UNZ_CRCERROR:
				return "CRC Error";
			default:
				return "Unknown Error";
		}
	}

	static inline void unz_check(int error) {
		if (error != UNZ_OK)
			throw Exception("Unzip error " + unzip_error_tostring(error));
	}

	/** Gets the name of the current file from the zip file. */
	static char* getCurrentName(unzFile file) {
		unz_file_info info;
		unz_check(unzGetCurrentFileInfo(file, &info,
			NULL, 0, NULL, 0, NULL, 0));

		char* buffer = new char[info.size_filename + 1];
		unz_check(unzGetCurrentFileInfo(file, NULL,
			buffer, info.size_filename + 1, NULL, 0, NULL, 0));

		return buffer;
	}

	static void checkRootDir(file_list_t& list, std::string& path) {
		int loc = path.find_first_of("/");
		if (loc > 0) {
			std::string rootDir = path.substr(0, loc + 1);
			if (std::find(list.begin(), list.end(), rootDir) == list.end())
				list.push_back(rootDir);
		}
	}

	static void populateFileList(file_list_t& list, unzFile file) {
		unz_check(unzGoToFirstFile(file));
		do {
			char* name = getCurrentName(file);
			std::string path(name);
			checkRootDir(list, path);
			list.push_back(path);
			delete[] name;
		} while (unzGoToNextFile(file) == UNZ_OK);
	}

	ZipSource::ZipSource(const std::string& zip_file) {
		m_data = new zip_data_t();

		m_data->zipName = zip_file;

		m_data->file = unzOpen(zip_file.c_str());
		if (m_data->file == NULL)
			throw FIFE::CannotOpenFile(zip_file);

		populateFileList(m_data->fileList, m_data->file);
	}

	ZipSource::~ZipSource() {
		unzClose(m_data->file);
		delete m_data;
	}
	
	bool ZipSource::fileExists(const std::string& file) const {
		//return unzLocateFile(m_data->file, file.c_str(), 2) == UNZ_OK;
		return
			std::find(m_data->fileList.begin(), m_data->fileList.end(), file)
			!= m_data->fileList.end();
	}

	RawData* ZipSource::open(const std::string& file) const {
		if (fileExists(file))
			return new RawData(new ZipData(m_data->zipName, file));
		else
			throw NotFound(file);
	}

	std::vector<std::string> ZipSource::listFiles(const std::string& path) const {
		std::vector<std::string> result;

		std::string fixedPath = fixPath(path);
		int path_len = path.length();
		if (fixedPath[path_len - 1] != '/') {
			fixedPath += '/';
			path_len++;
		}

		file_list_iter_t iter;
		for (iter = m_data->fileList.begin();
			iter != m_data->fileList.end(); iter++) {
				std::string name = *iter;
				int len = name.length();
				if (name.find(fixedPath) == 0 && name[len - 1] != '/')
					result.push_back(name.substr(path_len));
		}
		return result;
	}

	std::vector<std::string> ZipSource::listDirectories(const std::string& path) const {
		std::vector<std::string> result;

		std::string fixedPath = fixPath(path);
		int path_len = path.length();
		if (fixedPath[path_len - 1] != '/') {
			fixedPath += '/';
			path_len++;
		}

		file_list_iter_t iter;
		for (iter = m_data->fileList.begin();
			iter != m_data->fileList.end(); iter++) {
				std::string name = *iter;
				int len = name.length();
				if (name.find(fixedPath) == 0 && name[len - 1] == '/' && len > path_len)
					result.push_back(name.substr(path_len));
		}
		return result;
	}
}
