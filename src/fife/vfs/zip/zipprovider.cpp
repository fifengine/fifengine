// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "zipprovider.h"

// Standard C++ library includes
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>

// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"
#include "vfs/filesystem.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "zipsource.h"

namespace FIFE
{
    ZipProvider::~ZipProvider() = default;

    bool ZipProvider::isReadable(std::string const & file) const
    {
        // The file name should have a ".zip" extension, ignoring case:
        std::string extension = GetExtension(file);
        std::ranges::transform(extension, extension.begin(), [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });
        if (extension != ".zip") {
            return false;
        }

        // File should exist
        if (!getVFS()->exists(file)) {
            return false;
        }

        // File should pass basic ZIP signature check
        //
        // Common ZIP files start with the local file header signature
        // "PK\x03\x04" (0x04034b50).
        //
        // An empty ZIP archive consists only of the end-of-central-directory
        // record, starting with "PK\x05\x06" (0x06054b50).
        //
        // This is only a heuristic: valid self-extracting ZIP archives or
        // archives with prepended data may start with other bytes.
        //
        // A structural validation is performed by ZipSource::readIndex().
        std::unique_ptr<RawData> data;
        try {
            data = getVFS()->open(file);
        } catch (Exception const &) {
            return false;
        }

        if (!data || data->getDataLength() < 4) {
            return false;
        }

        uint32_t const magic = data->read32Little();

        return magic == 0x04034b50 || magic == 0x06054b50;
    }

    FIFE::VFSSource* ZipProvider::createSource(std::string const & file)
    {
        if (isReadable(file)) {
            VFSSource* source = nullptr;
            if (hasSource(file)) {
                source = m_sources[file].get();
            } else {
                auto newSource  = std::make_unique<ZipSource>(getVFS(), file);
                source          = newSource.get();
                m_sources[file] = std::move(newSource);
            }
            return source;
        }
        throw Exception("File " + file + " is not readable.");
    }

    VFSSource* ZipProvider::getSource(std::string const & path) const
    {
        VFSSource* source = nullptr;
        if (hasSource(path)) {
            source = m_sources.find(path)->second.get();
        }
        return source;
    }

    bool ZipProvider::hasSource(std::string const & path) const
    {
        return m_sources.contains(path);
    }
} // namespace FIFE
