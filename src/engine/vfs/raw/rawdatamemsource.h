// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_RAW_RAWDATAMEMSOURE_H
#define FIFE_VFS_RAW_RAWDATAMEMSOURE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include "util/base/fife_stdint.h"

// 3rd party library includes

// FIFE includes
#include "rawdatasource.h"

namespace FIFE
{

    /**
     *	Simpe RawDataSource that reads data from raw memory
     *
     * @note This Class is @b not incomplete. Use @c getRawData to
     * write into the Buffer.
     *
     * @see RawDataSource
     */
    class /*FIFE_API*/ RawDataMemSource : public RawDataSource
    {
        public:
            /**
             *	Create a new RawDataMemSource that allocates datalen bytes.
             * @param datalen the datalen to allocate
             */
            explicit RawDataMemSource(uint32_t datalen);
            ~RawDataMemSource() override;

            RawDataMemSource(RawDataMemSource const &)            = delete;
            RawDataMemSource& operator=(RawDataMemSource const &) = delete;

            uint32_t getSize() const override;
            void readInto(uint8_t* buffer, uint32_t start, uint32_t length) override;
            uint8_t* getRawData() const;

        private:
            uint8_t* m_data;
            uint32_t m_datalen;
    };

} // namespace FIFE

#endif
