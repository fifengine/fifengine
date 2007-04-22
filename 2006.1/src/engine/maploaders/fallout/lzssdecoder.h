/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef FIFELZSSDECODER_H
#define FIFELZSSDECODER_H

#include "vfs.h"
#include <stdint.h>

namespace FIFE {
	/** A helper class for unpacking LZSS encoded data 
	 * @bug just a quick&dirty wrapper around anchorites implementation - needs to be replaced with our own LZSS implementation
	 */
	class LZSSDecoder {
		public:
			/** Constructor
			 */
			LZSSDecoder();

			/** Destructor
			 */
			~LZSSDecoder();

			/** Decodes from a file into a pointer.
			 * @param input The VFS file to read from
			 * @param output The memory location to write to
			 * @param outputsize The size of the memory location in byte
			 */
			void decode(RawDataPtr input, uint8_t* output, const uint32_t outputsize);

		private:
			uint32_t m_outlen;
			uint32_t m_outindex;

			bool LZSSDecoder::LZSSDecode(RawDataPtr in, long nInputLength, uint8_t* out);

			int getByte(RawDataPtr in, long& inlen) const;
			bool putByte(uint8_t c, uint8_t* out);
			bool FlushBuffer(uint8_t* out);

	};

}

#endif
