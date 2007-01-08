/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

#ifndef FIFE_MAP_LOADERS_FALLOUT_LZSSDECODER_H
#define FIFE_MAP_LOADERS_FALLOUT_LZSSDECODER_H

// Standard C++ library includes

// Platform specific includes
#include "util/fifeint.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfs.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {
	/** A helper class for unpacking LZSS encoded data 
	 */
	class LZSSDecoder {
		public:
			/** Decodes from a file into a pointer.
			 * @param input The VFS file to read from
			 * @param output The memory location to write to
			 * @param outputsize The size of the memory location in byte
			 */
			void decode(RawDataPtr input, uint8_t* output, const uint32_t outputsize);

		private:
			uint32_t m_outlen;
			uint32_t m_outindex;

			bool LZSSDecode(uint8_t* in, long nInputLength, uint8_t* out);

			void putByte(uint8_t c, uint8_t* out) {
				out[m_outindex++] = c;
			}
	};

} } } }

#endif
