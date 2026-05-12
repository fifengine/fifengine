// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_ZIP_ENTRY_H
#define FIFE_ZIP_ENTRY_H

#include <cstdint>
#include <string>

namespace FIFE
{

    /**
     * The type of entry in a zip archive.
     *
     * Classification is based on the central directory entry:
     * - trailing '/' in the filename indicates a directory
     * - Unix external attributes bit 0x4000 (S_IFDIR) indicates a directory
     * - Unix external attributes mask 0xA000 (S_IFLNK) indicates a symlink
     * - otherwise the entry is a file
     */
    enum class ZipEntryType : uint8_t
    {
        File,      // regular file entry
        Directory, // directory entry
        Symlink    // symbolic link entry
    };

    /**
     * Metadata about a zip archive entry read from the central directory.
     */
    struct ZipEntryInfo
    {
            std::string path;             // full path within the archive (may include trailing '/')
            ZipEntryType type;            // file, directory, or symlink
            uint32_t uncompressed_size;   // uncompressed data size in bytes
            uint32_t compressed_size;     // compressed data size in bytes
            uint32_t crc32;               // CRC-32 checksum of the uncompressed data
            uint32_t local_header_offset; // offset of the local file header from the start of the archive
            uint16_t compression;         // compression method (0 = stored, 8 = deflated)
    };

} // namespace FIFE

#endif
