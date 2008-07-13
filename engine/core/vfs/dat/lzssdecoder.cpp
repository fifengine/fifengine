/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes

// FIFE includes
#include "vfs/raw/rawdata.h"
#include "util/base/exception.h"

#include "lzssdecoder.h"

// just a quick&dirty wrapper around anchorites implementation - needs to be replaced with our own LZSS implementation
namespace FIFE {

	LZSSDecoder::LZSSDecoder() {}
	LZSSDecoder::~LZSSDecoder() {}


	void LZSSDecoder::decode(RawData* input, uint8_t* output, const uint32_t outputsize) {

		m_outindex = 0;
		m_outlen = outputsize;

		while (m_outindex < outputsize) {
			uint16_t blockdesc = input->read16Big();
			uint16_t bytesToRead = blockdesc & 0x7fff;

			if (blockdesc & 0x8000) { // uncompressed
				input->readInto(output + m_outindex, bytesToRead);
				m_outindex += bytesToRead;
			} else {
				// Allocate +2 bytes so that on corrupt data the LZSS
				// decoder won't crash the input buffer.
				std::vector<uint8_t> indata(bytesToRead + 2);
				input->readInto(&indata[0], bytesToRead);
				LZSSDecode(&indata[0], bytesToRead, output);
				// Note outindex is advanced inside LZSSDecode.
			}
			
		}
	}

	void LZSSDecoder::LZSSDecode(uint8_t* in , long len, uint8_t* out) {
		const long c_nRingBufferSize        = 4096;
		const long c_nMatchLengthUpperLimit =   18;
		const long c_nThreshold             =    2;

		char buffer[c_nRingBufferSize + c_nMatchLengthUpperLimit - 1];
		int ibuf = 0;
		int c;

		int i;
		int j;
		int k;
		int r;

		unsigned int flags;

		for (i = 0; i < c_nRingBufferSize - c_nMatchLengthUpperLimit; i++) {
			buffer[i] = ' ';
		}

		r = c_nRingBufferSize - c_nMatchLengthUpperLimit;
		flags = 0;
		while ( ibuf < len ) {
			if (((flags >>= 1) & 256) == 0) {
				c = in[ibuf++];
				flags = c | 0xff00;/* uses higher byte cleverly to count eight */
			}

			if (flags & 1) {
				c = in[ibuf++];
				out[m_outindex++] = c;

				buffer[r++] = c;
				r &= (c_nRingBufferSize - 1);
			} else {
				i = in[ibuf++];
				j = in[ibuf++];

				i |= ((j & 0xf0) << 4);
				j = (j & 0x0f) + c_nThreshold;

				for (k = 0; k <= j; k++) {
					c = buffer[(i + k) & (c_nRingBufferSize - 1)];

					out[m_outindex++] = c;

					buffer[r++] = c;
					r &= (c_nRingBufferSize - 1);
				}
			}
		}
	}


}
