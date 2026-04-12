// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_RAW_RAWDATASOURCE_H
#define FIFE_VFS_RAW_RAWDATASOURCE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include "util/base/fife_stdint.h"

// 3rd party library includes

// FIFE includes

namespace FIFE
{

    /** Abstract baseclass - provides data for RawData
     *
     * RawData uses RawDataSources to access the data - if you want to add support for a new archivetype this is one of
     * the classes you need.
     */
    class /*FIFE_API*/ RawDataSource
    {

    public:
        RawDataSource();
        virtual ~RawDataSource();

        /** get the complete datasize */
        virtual uint32_t getSize() const = 0;

        /** read data from the source
         *
         * @param buffer the data will be written into buffer
         * @param start the startindex inside the source
         * @param length length bytes will be written into buffer
         */
        virtual void readInto(uint8_t* buffer, uint32_t start, uint32_t length) = 0;
    };

} // namespace FIFE

#endif