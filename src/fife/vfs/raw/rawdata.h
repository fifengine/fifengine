// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_RAW_RAWDATA_H
#define FIFE_VFS_RAW_RAWDATA_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <array>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "util/base/fife_stdint.h"

// 3rd party library includes

// FIFE includes

#include "rawdatasource.h"
#include "util/base/exception.h"

namespace FIFE
{

    /** Used to access diffrent kinds of data.
     *
     * RawData uses RawDataSource to get the real data - that way the user doesn't have to know where the data comes
     * from (real files, files inside archives etc.)
     */
    class FIFE_API RawData
    {
        public:
            explicit RawData(RawDataSource* datasource);

            RawData(RawData const &)            = delete;
            RawData& operator=(RawData const &) = delete;
            virtual ~RawData();

            /** get the data as a vector of bytes
             * This does not append a null terminator to the end
             */
            std::vector<uint8_t> getDataInBytes();

            /** get the data in distinct lines
             */
            std::vector<std::string> getDataInLines();

            /** get the complete datalength
             *
             * @return the complete datalength
             */
            uint32_t getDataLength() const;

            /** get the current index
             *
             * @return the current index
             */
            uint32_t getCurrentIndex() const;

            /** set the current index
             *
             * @param index the new index
             * @throws IndexOverflow if index is >= getDataLength()
             */
            void setIndex(uint32_t index);

            /** move the current index
             *
             * @param offset the offset
             * @throws IndexOverflow if we move outside the datalength
             */
            void moveIndex(int32_t offset);

            /** helper-function
             *
             * reads sizeof(T) bytes - should be used with fixed-size datatypes like uint32_t, uint16_t, uint8_t etc.
             * @return the data
             */
            template <typename T>
            T readSingle()
            {
                T val{};
                readInto(static_cast<uint8_t*>(static_cast<void*>(&val)), sizeof(T));
                return val;
            }

            /** read len bytes into buffer
             *
             * @param buffer the data will be written into it
             * @param len len bytes will be written
             * @throws IndexOverflow if currentindex + len > getCurrentIndex()
             */
            void readInto(uint8_t* buffer, size_t len);

            /** reads 1 byte */
            uint8_t read8();

            /** reads a uint16_t littleEndian and converts them to the host-byteorder
             * @throws IndexOverflow
             */
            uint16_t read16Little();

            /** reads a uint16_t littleEndian and converts them to the host-byteorder
             *
             * @throws IndexOverflow
             */
            uint32_t read32Little();

            /** reads a uint16_t bigEndian and converts them to the host-byteorder
             *
             * @throws IndexOverflow
             */
            uint16_t read16Big();

            /** reads a uint16_t bigEndian and converts them to the host-byteorder
             *
             * @throws IndexOverflow
             */
            uint32_t read32Big();

            /** read a string with len bytes, not assuming a terminating 0
             * Appends a null terminator character to the end
             *
             * @param len the stringlen
             * @return the string
             * @throws IndexOverflow
             */
            std::string readString(size_t len);

            /** Reads all data into the buffer
             * This does not append a null terminator to the end
             * Created to especially fulfill python file interface requirements
             */
            void read(std::string& outbuffer, int32_t size = -1);

            /** reads until a \\n is encountered or no more data is available
             *
             * @param buffer if successfull the new string will be assigned to buffer
             * @return true if data was available, false otherwise (in that case buffer won't be touched)
             */
            bool getLine(std::string& buffer);

        private:
            RawDataSource* m_datasource;
            uint32_t m_index_current;

            template <typename T>
            T littleToHost(T value) const
            {
                if (littleEndian()) {
                    return value;
                }
                return revert(value);
            }

            template <typename T>
            T bigToHost(T value) const
            {
                if (!littleEndian()) {
                    return value;
                }
                return revert(value);
            }

            template <typename T>
            T revert(T value) const
            {
                std::array<uint8_t, sizeof(T)> inBytes{};
                std::array<uint8_t, sizeof(T)> outBytes{};
                std::memcpy(inBytes.data(), &value, sizeof(T));
                for (uint32_t i = 0; i < sizeof(T); ++i) {
                    outBytes[i] = inBytes[sizeof(T) - 1U - i];
                }

                T retval{};
                std::memcpy(&retval, outBytes.data(), sizeof(T));
                return retval;
            }

            static bool littleEndian();
    };
    using RawDataPtr = std::shared_ptr<RawData>;
    class FIFE_API IndexSaver
    {
        public:
            explicit IndexSaver(RawData* d) : m_rd(d), m_index(m_rd->getCurrentIndex())
            {
            }

            IndexSaver(IndexSaver const &)            = delete;
            IndexSaver& operator=(IndexSaver const &) = delete;

            ~IndexSaver()
            {
                try {
                    m_rd->setIndex(m_index);
                } catch (IndexOverflow const & err) {
                    (void)err;
                }
            }

        private:
            RawData* m_rd;
            uint32_t m_index;
    };

} // namespace FIFE

#endif
