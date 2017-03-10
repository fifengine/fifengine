/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_VFS_RAW_RAWDATA_H
#define FIFE_VFS_RAW_RAWDATA_H

// Standard C++ library includes
#include <vector>

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "rawdatasource.h"

namespace FIFE {

	/** Used to access diffrent kinds of data.
	 *
	 * RawData uses RawDataSource to get the real data - that way the user doesn't have to know where the data comes
	 * from (real files, files inside archives etc.)
	 */
	class RawData {
		public:
			RawData(RawDataSource* datasource);
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
			template <typename T> T readSingle() {
				T val;
				readInto(reinterpret_cast<uint8_t*>(&val), sizeof(T));
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
			void read(std::string& outbuffer, int32_t size=-1);
			
			/** reads until a \\n is encountered or no more data is available
			 *
			 * @param buffer if successfull the new string will be assigned to buffer
			 * @return true if data was available, false otherwise (in that case buffer won't be touched)
			 */
			bool getLine(std::string& buffer);

		private:
			RawDataSource* m_datasource;
			size_t m_index_current;

			template <typename T> T littleToHost(T value) const {
				if (littleEndian())
					return value;
				else
					return revert(value);
			}

			template <typename T> T bigToHost(T value) const {
				if (!littleEndian())
					return value;
				else
					return revert(value);
			}

			template <typename T> T revert(T value) const {
				T retval;
				for (uint32_t i = 0; i < sizeof(T); ++i)
					reinterpret_cast<uint8_t*>(&retval)[i] = reinterpret_cast<uint8_t*>(&value)[sizeof(T)-1-i];

				return retval;
			}

			RawData(const RawData&);
			RawData& operator=(const RawData&) { return *this; };

			static bool littleEndian();
	};
	typedef boost::shared_ptr<RawData> RawDataPtr;

	class IndexSaver {
		public:
			IndexSaver(RawData* d) : m_rd(d), m_index(m_rd->getCurrentIndex()) {}

			~IndexSaver() {
				m_rd->setIndex(m_index);
			}

		private:
			RawData* m_rd;
			uint32_t m_index;

			IndexSaver(const IndexSaver&);
			IndexSaver& operator=(const IndexSaver&) { return *this; }
	};

}//FIFE

#endif
