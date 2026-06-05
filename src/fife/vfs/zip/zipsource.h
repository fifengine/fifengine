// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_ZIP_SOURCE_H
#define FIFE_ZIP_SOURCE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <memory>
#include <set>
#include <string>

// 3rd party library includes

// FIFE includes
#include "vfs/vfssource.h"
#include "vfs/zip/zipentry.h"
#include "ziptree.h"

namespace FIFE
{
    class RawData;
    class VFS;
} // namespace FIFE

namespace FIFE
{
    /**  Implements a Zip archive file source.
     *
     * @see FIFE::VFSSource
     */
    class FIFE_API ZipSource : public VFSSource
    {
        public:
            ZipSource(VFS* vfs, std::string const & zip_file);
            ~ZipSource() override;

            ZipSource(ZipSource const &)            = delete;
            ZipSource& operator=(ZipSource const &) = delete;

            /**
             * WARNING: fileExists, listFiles and listDirectories are not
             * thread-safe, and will probably break if called from multiple
             * threads at the same time.
             */
            bool fileExists(std::string const & file) const override;
            std::set<std::string> listFiles(std::string const & path) const override;
            std::set<std::string> listDirectories(std::string const & path) const override;

            std::unique_ptr<RawData> open(std::string const & path) const override;

        private:
            /**
             * Reads the zip archive index by parsing the central directory.
             *  Calls readEndOfCentralDirectory() to locate the EOCD record,
             *  then iterates readCentralDirectoryEntry() for each entry.
             */
            void readIndex();

            /**
             * Locates and parses the End of Central Directory record.
             *  Reads from the end of the file backward to find the EOCD signature.
             *  Sets m_centralDirOffset and m_centralDirCount.
             */
            void readEndOfCentralDirectory();

            /**
             * Reads one central directory entry at the current file position.
             *  Extracts filename, compression, sizes, CRC, and external attributes,
             *  classifies the entry using classifyEntry(), and adds it to m_zipTree.
             *  For file entries, the ZipEntryData is stored on the node.
             */
            void readCentralDirectoryEntry();

            /**
             * Computes the offset of the file data (after the local file header).
             *  Reads the local file header at the given offset to determine
             *  the filename and extra field lengths, then returns the data start.
             *  @param localHeaderOffset offset of the local file header
             *  @return offset of the compressed/stored data
             */
            uint32_t getLocalFileDataOffset(uint32_t localHeaderOffset) const;

            /**
             * Classifies a zip entry as File, Directory, or Symlink.
             *
             *  Classification rules (checked in order):
             *  1. Trailing '/' in the filename  -> Directory
             *  2. Unix S_IFDIR (0x4000) bit set  -> Directory
             *  3. Unix S_IFLNK (0xA000) mask     -> Symlink
             *  4. Otherwise                      -> File
             *
             *  @param name the filename from the central directory entry
             *  @param externalAttrs the external file attributes (Unix mode)
             *  @return the classified entry type
             */
            ZipEntryType classifyEntry(std::string const & name, uint32_t externalAttrs) const;

            ZipTree m_zipTree;
            std::unique_ptr<RawData> m_zipfile;
            uint32_t m_centralDirOffset;
            uint16_t m_centralDirCount;
    };

} // namespace FIFE

#endif
