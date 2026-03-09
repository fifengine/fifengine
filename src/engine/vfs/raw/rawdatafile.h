// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_RAW_RAWDATAFILE_H
#define FIFE_VFS_RAW_RAWDATAFILE_H

// Standard C++ library includes
#include <fstream>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "rawdatasource.h"

namespace FIFE
{

    /** A RawDataSource for a file on the host system
     * @see VFSHostSystem
     * @see RawDataSource
     */
    class RawDataFile : public RawDataSource
    {

    public:
        /** Constructor
         * Constructs a RawDataSource for file.
         * @param file The path to the file to load.
         * @throw CannotOpenFile
         */
        RawDataFile(std::string  file);
        virtual ~RawDataFile();

        virtual uint32_t getSize() const;
        virtual void readInto(uint8_t* buffer, uint32_t start, uint32_t length);

    private:
        std::string m_file;
        std::ifstream m_stream;

        uint32_t m_filesize;
    };

} // namespace FIFE

#endif