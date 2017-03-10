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

#ifndef FIFE_LZSSDECODER_H
#define FIFE_LZSSDECODER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "util/base/fife_stdint.h"
#include "vfs/vfs.h"

namespace FIFE {
	/** A helper class for unpacking LZSS encoded data
	 * @bug just a quick&dirty LZSS implementation, optimized a bit
	 *      see revision 719 for the original one and 795 for the broken optimized one
	 * @bug the lzss decoder might very well crash on corrupt data.
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
			void decode(RawData* input, uint8_t* output, const uint32_t outputsize);

		private:
			uint32_t m_outlen;
			uint32_t m_outindex;
			void LZSSDecode(uint8_t* in, int64_t len, uint8_t* out);

	};

}

#endif
