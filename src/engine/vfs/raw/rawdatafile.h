// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_RAW_RAWDATAFILE_H
#define FIFE_VFS_RAW_RAWDATAFILE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <fstream>
#include <string>

// 3rd party library includes

// FIFE includes
#include "rawdatasource.h"

namespace FIFE
{

    /** A RawDataSource for a file on the host system
     * @see VFSHostSystem
     * @see RawDataSource
     */
    class FIFE_API RawDataFile : public RawDataSource
    {

    public:
        /** Constructor
         * Constructs a RawDataSource for file.
         * @param file The path to the file to load.
         * @throw CannotOpenFile
         */
        explicit RawDataFile(std::string file);
        ~RawDataFile() override;

        RawDataFile(const RawDataFile&)            = delete;
        RawDataFile& operator=(const RawDataFile&) = delete;

        uint32_t getSize() const override;
        void readInto(uint8_t* buffer, uint32_t start, uint32_t length) override;

    private:
        std::string m_file;
        std::ifstream m_stream;

        uint32_t m_filesize;
    };

} // namespace FIFE

#endif
