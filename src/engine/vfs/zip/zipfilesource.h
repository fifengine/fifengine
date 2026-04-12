// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_ZIP_ZIPFILESOURCE_H
#define FIFE_VFS_ZIP_ZIPFILESOURCE_H

// Platform specific includes
#include "platform.h"

#include "vfs/raw/rawdatasource.h"
#include <cstdint>

namespace FIFE
{

    class /*FIFE_API*/ ZipFileSource : public RawDataSource
    {
    public:
        ZipFileSource(uint8_t* data, uint32_t datalen);
        virtual ~ZipFileSource();

        virtual uint32_t getSize() const;
        virtual void readInto(uint8_t* target, uint32_t start, uint32_t len);

    private:
        uint8_t* m_data;
        uint32_t m_datalen;
    };

} // namespace FIFE

#endif
