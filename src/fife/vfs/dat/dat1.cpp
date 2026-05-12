// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "dat1.h"

// Standard C++ library includes
#include <list>
#include <set>
#include <string>
#include <utility>

// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "vfs/raw/rawdata.h"

namespace FIFE
{
    static Logger _log(LM_FO_LOADERS);

    DAT1::DAT1(VFS* vfs, std::string const & file) : VFSSource(vfs), m_datpath(file), m_data(vfs->open(file))
    {
        FL_LOG(_log, LMsg("MFFalloutDAT1") << "loading: " << file << " filesize: " << m_data->getDataLength());

        m_data->setIndex(0);

        uint32_t const dircount = m_data->read32Big();
        m_data->moveIndex(4 * 3);

        FL_LOG(_log, LMsg("MFFalloutDAT1") << "number of directories " << dircount);

        // Sanity check. Each dir entry needs min. 16 bytes.
        if (dircount * 16 > m_data->getDataLength()) {
            throw InvalidFormat("directory count larger than filesize.");
        }

        std::list<std::string> dir_names;
        for (uint32_t i = 0; i < dircount; ++i) {
            std::string name = readString();
            if (name == ".") {
                name = "";
            }
            dir_names.push_back(name);
        }

        for (auto& dir_name : dir_names) {
            loadFileList(dir_name);
        }
    }

    void DAT1::loadFileList(std::string const & dirname)
    {
        uint32_t const filecount = m_data->read32Big();
        m_data->moveIndex(4 * 3);
        for (uint32_t i = 0; i < filecount; ++i) {
            RawDataDAT1::s_info info;
            info.name           = fixPath(dirname + "/" + readString());
            info.type           = m_data->read32Big();
            info.offset         = m_data->read32Big();
            info.unpackedLength = m_data->read32Big();
            info.packedLength   = m_data->read32Big();

            m_filelist.insert(std::make_pair(info.name, info));
        }
    }

    std::string DAT1::readString()
    {
        uint8_t const length = m_data->read8();
        return m_data->readString(length);
    }

    RawData* DAT1::open(std::string const & file) const
    {
        RawDataDAT1::s_info const & info = getInfo(file);
        return new RawData(new RawDataDAT1(getVFS(), m_datpath, info));
    }

    bool DAT1::fileExists(std::string const & name) const
    {
        return m_filelist.contains(name);
    }

    RawDataDAT1::s_info const & DAT1::getInfo(std::string const & name) const
    {
        auto i = m_filelist.find(name);
        if (i == m_filelist.end()) {
            throw NotFound(name);
        }

        return i->second;
    }

    std::set<std::string> DAT1::listFiles(std::string const & pathstr) const
    {
        return list(pathstr, false);
    }

    std::set<std::string> DAT1::listDirectories(std::string const & pathstr) const
    {
        return list(pathstr, true);
    }

    std::set<std::string> DAT1::list(std::string const & pathstr, bool dirs) const
    {
        std::set<std::string> list;
        std::string path = pathstr;

        // Normalize the path
        if (path.starts_with("./")) {
            path.erase(0, 2);
        }

        size_t const lastIndex = path.size();
        if (lastIndex != 0 && path[lastIndex - 1] != '/') {
            path += '/';
        }

        auto end = m_filelist.end();
        for (auto i = m_filelist.begin(); i != end; ++i) {
            std::string const & file = i->first;
            if (file.starts_with(path)) {
                std::string cleanedfile = file.substr(path.size(), file.size()); // strip the pathstr
                bool const isdir = cleanedfile.find('/') != std::string::npos;   // if we still have a / it's a subdir

                if (isdir) {
                    cleanedfile.resize(cleanedfile.find('/'));
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
} // namespace FIFE
