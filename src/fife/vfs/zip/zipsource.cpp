// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "zipsource.h"

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <format>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "modules.h"
#include "util/log/logger.h"
#include "vfs/filesystem.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "vfs/zip/ziptree.h"
#include "zipfilesource.h"
#include "zipnode.h"
#include "zlib.h"

namespace FIFE
{

    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_LOADERS);
            return log;
        }
    } // namespace

    ZipSource::ZipSource(VFS* vfs, std::string const & zip_file) :
        VFSSource(vfs), m_zipfile(vfs->open(zip_file)), m_centralDirOffset(0), m_centralDirCount(0)
    {
        readIndex();
    }

    ZipSource::~ZipSource() = default;

    bool ZipSource::fileExists(std::string const & file) const
    {
        fs::path const path(file);
        return m_zipTree.getNode(path.string()) != nullptr;
    }

    std::unique_ptr<RawData> ZipSource::open(std::string const & path) const
    {
        fs::path const filePath(path);
        ZipNode const * node = m_zipTree.getNode(filePath.string());

        assert("File not found in zip archive" && node != nullptr);

        if (node != nullptr) {
            ZipEntryData const & entryData = node->getZipEntryData();
            uint32_t const dataOffset      = getLocalFileDataOffset(entryData.offset);

            m_zipfile->setIndex(dataOffset);
            auto data = std::make_unique<uint8_t[]>(entryData.size_real); // NOLINT(cppcoreguidelines-avoid-c-arrays,
                                                                          // modernize-avoid-c-arrays)
            if (entryData.comp == 8) {
                FL_DBG(
                    _log(),
                    std::format("trying to uncompress file {} (compressed with method {})", path, entryData.comp));
                std::vector<uint8_t> compdata(entryData.size_comp);
                m_zipfile->readInto(compdata.data(), entryData.size_comp);

                z_stream zstream;
                zstream.next_in   = compdata.data();
                zstream.avail_in  = entryData.size_comp;
                zstream.zalloc    = Z_NULL;
                zstream.zfree     = Z_NULL;
                zstream.opaque    = Z_NULL;
                zstream.next_out  = data.get();
                zstream.avail_out = entryData.size_real;

                if (inflateInit2(&zstream, -15) != Z_OK) {
                    FL_ERR(_log(), "inflateInit2 failed");
                    return nullptr;
                }

                int32_t const err = inflate(&zstream, Z_FINISH);
                if (err != Z_STREAM_END) {
                    if (zstream.msg != nullptr) {
                        FL_ERR(_log(), std::format("inflate failed: {}", zstream.msg));
                    } else {
                        FL_ERR(_log(), std::format("inflate failed without msg, err: {}", err));
                    }
                    inflateEnd(&zstream);
                    return nullptr;
                }

                inflateEnd(&zstream);
            } else if (entryData.comp == 0) {
                m_zipfile->readInto(data.get(), entryData.size_real);
            } else {
                FL_ERR(_log(), "unsupported compression");
                return nullptr;
            }

            return std::make_unique<RawData>(
                new ZipFileSource(data.release(), entryData.size_real)); // NOLINT(cppcoreguidelines-owning-memory)
        }

        return nullptr;
    }

    void ZipSource::readIndex()
    {
        readEndOfCentralDirectory();

        m_zipfile->setIndex(m_centralDirOffset);
        for (uint32_t i = 0; i < m_centralDirCount; ++i) {
            readCentralDirectoryEntry();
        }
    }

    void ZipSource::readEndOfCentralDirectory()
    {
        uint32_t const fileSize = m_zipfile->getDataLength();
        assert("Zip file is too small" && fileSize >= 22);

        m_zipfile->setIndex(fileSize - 2);
        uint16_t const commentLength = m_zipfile->read16Little();

        uint32_t const eocdOffset = fileSize - 22 - commentLength;
        m_zipfile->setIndex(eocdOffset);
        uint32_t const signature = m_zipfile->read32Little();
        assert("End of Central Directory signature not found" && signature == 0x06054b50);

        m_zipfile->read16Little();                      // disk number
        m_zipfile->read16Little();                      // disk with CD
        m_zipfile->read16Little();                      // entries on this disk
        m_centralDirCount = m_zipfile->read16Little();  // total entries
        m_zipfile->read32Little();                      // CD size
        m_centralDirOffset = m_zipfile->read32Little(); // CD offset
    }

    void ZipSource::readCentralDirectoryEntry()
    {
        uint32_t const signature = m_zipfile->read32Little();
        assert("Invalid central directory entry signature" && signature == 0x02014b50);

        m_zipfile->read16Little(); // version made by
        m_zipfile->read16Little(); // version needed
        m_zipfile->read16Little(); // flags
        uint16_t const compression = m_zipfile->read16Little();
        m_zipfile->read16Little(); // mod time
        m_zipfile->read16Little(); // mod date
        uint32_t const crc32            = m_zipfile->read32Little();
        uint32_t const compressedSize   = m_zipfile->read32Little();
        uint32_t const uncompressedSize = m_zipfile->read32Little();
        uint16_t const filenameLen      = m_zipfile->read16Little();
        uint16_t const extraLen         = m_zipfile->read16Little();
        uint16_t const commentLen       = m_zipfile->read16Little();
        m_zipfile->read16Little(); // disk number start
        m_zipfile->read16Little(); // internal attrs
        uint32_t const externalAttrs     = m_zipfile->read32Little();
        uint32_t const localHeaderOffset = m_zipfile->read32Little();

        std::string const filename = m_zipfile->readString(filenameLen);

        m_zipfile->moveIndex(extraLen + commentLen);

        ZipEntryType const type = classifyEntry(filename, externalAttrs);

        ZipNode* node = m_zipTree.addNode(filename, type);
        assert("Failed to add node to zip tree" && node != nullptr);

        if (type == ZipEntryType::File) {
            ZipEntryData data;
            data.comp      = compression;
            data.size_real = uncompressedSize;
            data.size_comp = compressedSize;
            data.offset    = localHeaderOffset;
            data.crc32     = crc32;
            node->setZipEntryData(data);
        }
    }

    uint32_t ZipSource::getLocalFileDataOffset(uint32_t localHeaderOffset) const
    {
        m_zipfile->setIndex(localHeaderOffset + 26);
        uint16_t const fnamelen = m_zipfile->read16Little();
        uint16_t const extralen = m_zipfile->read16Little();
        return localHeaderOffset + 30 + fnamelen + extralen;
    }

    ZipEntryType ZipSource::classifyEntry(std::string const & name, uint32_t externalAttrs) const
    {
        if (!name.empty() && name.back() == '/') {
            return ZipEntryType::Directory;
        }
        if ((externalAttrs & 0x4000) != 0U) {
            return ZipEntryType::Directory;
        }
        if ((externalAttrs & 0xF000) == 0xA000) {
            return ZipEntryType::Symlink;
        }
        return ZipEntryType::File;
    }

    std::set<std::string> ZipSource::listFiles(std::string const & path) const
    {
        std::set<std::string> result;

        ZipNode const * node = m_zipTree.getNode(path);

        if (node != nullptr) {
            auto const files = node->getChildren(ZipContentType::File);
            for (auto const * child : files) {
                result.insert(child->getName());
            }
        }

        return result;
    }

    std::set<std::string> ZipSource::listDirectories(std::string const & path) const
    {
        std::set<std::string> result;

        ZipNode const * node = m_zipTree.getNode(path);

        if (node != nullptr) {
            auto const dirs = node->getChildren(ZipContentType::Directory);
            for (auto const * child : dirs) {
                result.insert(child->getName());
            }
        }

        return result;
    }
} // namespace FIFE
