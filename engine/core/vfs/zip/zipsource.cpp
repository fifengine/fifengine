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

// Standard C++ library includes
#include <algorithm>
#include <list>

// 3rd party library includes
#include "zlib.h"
#include <boost/scoped_array.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "util/base/exception.h"
#include "util/log/logger.h"

#include "zipsource.h"
#include "zipfilesource.h"

namespace FIFE {

	static const uint32_t LF_HEADER = 0x04034b50;
	static const uint32_t DE_HEADER = 0x08064b50;
	static const uint32_t CF_HEADER = 0x02014b50;

	static Logger _log(LM_LOADERS);

	ZipSource::ZipSource(VFS* vfs, const std::string& zip_file) : VFSSource(vfs),  m_zipfile(vfs->open(zip_file)) {
		readIndex();
	}

	ZipSource::~ZipSource() {
		delete m_zipfile;
	}

	bool ZipSource::fileExists(const std::string& file) const {
		return m_files.find(file) != m_files.end();
	}

	RawData* ZipSource::open(const std::string& path) const {
		type_files::const_iterator i = m_files.find(path);
		assert(i != m_files.end());
		const s_data& info = i->second;

		m_zipfile->setIndex(info.offset);
		uint8_t* data = new uint8_t[info.size_real]; // beware of me - one day i WILL cause memory leaks
		if (info.comp == 8) { // compressed using deflate
			FL_DBG(_log, LMsg("trying to uncompress file ") <<  path << " (compressed with method " << info.comp << ")");
			boost::scoped_array<uint8_t> compdata(new uint8_t[info.size_comp]);
			m_zipfile->readInto(compdata.get(), info.size_comp);

			z_stream zstream;
			zstream.next_in = compdata.get();
			zstream.avail_in = info.size_comp;
			zstream.zalloc = Z_NULL;
			zstream.zfree = Z_NULL;
			zstream.opaque = Z_NULL;
			zstream.next_out = data;
			zstream.avail_out = info.size_real;

			if (inflateInit2(&zstream, -15) != Z_OK) {
				FL_ERR(_log, LMsg("inflateInit2 failed"));
				delete[] data;
				return 0;
			}

			int err = inflate(&zstream, Z_FINISH);
			if (err != Z_STREAM_END) {
				if (zstream.msg) {
					FL_ERR(_log, LMsg("inflate failed: ") << zstream.msg);
				} else {
					FL_ERR(_log, LMsg("inflate failed without msg, err: ") << err);
				}

				inflateEnd(&zstream);
				delete[] data;
				return 0;
			}

			inflateEnd(&zstream);
		} else if (info.comp == 0) { // uncompressed
			m_zipfile->readInto(data, info.size_real);
		} else {
			FL_ERR(_log, LMsg("unsupported compression"));
			return 0;
		}

		return new RawData(new ZipFileSource(data, info.size_real));
	}

	void ZipSource::readIndex() {
		m_zipfile->setIndex(0);
		m_files.clear();

		while (!readFileToIndex()) {}
	}

	bool ZipSource::readFileToIndex() {
		uint32_t header   = m_zipfile->read32Little();
		if (header == DE_HEADER || header == CF_HEADER) { // decryption header or central directory header - we are finished
			return true;
		}

		uint16_t vneeded  = m_zipfile->read16Little();
		uint16_t gflags   = m_zipfile->read16Little();
		uint16_t comp     = m_zipfile->read16Little();
		uint16_t lmodtime = m_zipfile->read16Little();
		uint16_t lmoddate = m_zipfile->read16Little();
		uint32_t crc      = m_zipfile->read32Little();
		uint32_t compsize = m_zipfile->read32Little();
		uint32_t realsize = m_zipfile->read32Little();
		uint16_t fnamelen = m_zipfile->read16Little();
		uint16_t extralen = m_zipfile->read16Little();

		if (header != LF_HEADER) {
			FL_ERR(_log, LMsg("invalid local file header: ") << header);
			return true;
		}

		if (vneeded > 20) {
			FL_ERR(_log, LMsg("only zip version 2 is supported, required: ") << vneeded);
			return true;
		}

		std::string filename = m_zipfile->readString(fnamelen);
		m_zipfile->moveIndex(extralen);
		unsigned int offset = m_zipfile->getCurrentIndex();
		FL_DBG(_log, LMsg("found file: ") << filename << " (" << compsize << "/" << realsize << ") on offset " << offset);

		m_zipfile->moveIndex(compsize);
		if (gflags & (0x01 << 3)) {
			crc = m_zipfile->read32Little();
			compsize = m_zipfile->read32Little();
			realsize = m_zipfile->read32Little();
		}

		if (lmodtime || lmoddate) {} // shut up the compiler (warnings of unused variables)

		s_data data;
		data.comp = comp;
		data.size_real = realsize;
		data.size_comp = compsize;
		data.offset = offset;
		data.path = filename;
		data.crc32 = crc;

		m_files[filename] = data;
		return false;
	}


	// FIXME: quick&very dirty..
	std::set<std::string> ZipSource::listFiles(const std::string& path) const {
		std::set<std::string> result;

		std::string fixedPath = fixPath(path);
		int path_len = path.length();
		if (fixedPath[path_len - 1] != '/') {
			fixedPath += '/';
			path_len++;
		}

		type_files::const_iterator end = m_files.end();
		for (type_files::const_iterator i = m_files.begin(); i != end; ++i) {
			std::string name = i->first;
			int len = name.length();
			if (len && name.find(fixedPath) == 0 && name[len - 1] != '/') {
				name = name.substr(path_len);
				size_t pos = name.find("/");
				if (pos != std::string::npos)
					continue;

				result.insert(name);
			}
		}

		return result;
	}

	// FIXME: quick&very dirty..
	std::set<std::string> ZipSource::listDirectories(const std::string& path) const {
		std::set<std::string> result;

		std::string fixedPath = fixPath(path);
		int path_len = path.length();
		if (fixedPath[path_len - 1] != '/') {
			fixedPath += '/';
			path_len++;
		}

		type_files::const_iterator end = m_files.end();
		for (type_files::const_iterator i = m_files.begin(); i != end; ++i) {
			std::string name = i->first;
			int len = name.length();
			if (len && name.find(fixedPath) == 0 && name[len - 1] == '/' && len > path_len) {
				name = name.substr(path_len);
				size_t pos = name.find("/");
				if (pos != std::string::npos) {
					name = name.substr(0, pos);
				}
				result.insert(name);
			}
		}

		return result;
	}
}
