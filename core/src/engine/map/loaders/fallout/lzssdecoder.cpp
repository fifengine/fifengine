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

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "exception.h"
#include "log.h"

#include "lzssdecoder.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {

	static const uint32_t RING_SIZE = 4096;
	static const uint32_t THRESHOLD = 2;

	void LZSSDecoder::decode(RawDataPtr input, uint8_t* output, const uint32_t outputsize) {

		m_outindex = 0;
		m_outlen = outputsize;
		
		while (m_outindex < outputsize) {
			uint16_t blockdesc = input->read16Big();
			uint16_t bytesToRead = blockdesc & 0x7fff;

			if (blockdesc & 0x8000) { // uncompressed
				input->readInto(output, bytesToRead);
				m_outindex += bytesToRead;
			} else {
				size_t dlen = input->getDataLength();

				if (dlen < bytesToRead) {
					Warn( __FUNCTION__ ) << "Got " << bytesToRead << " to read, but only " << dlen << " total bytes!";

					return;
				}

				std::vector<uint8_t> indata(bytesToRead);
				input->readInto(&indata[0], bytesToRead);	
				LZSSDecode(&indata[0], bytesToRead, output);
			}
		}

		Debug( __FUNCTION__ ) << "Read resource: " << m_outindex << " total bytes";
	}

// Pseudocode blatantly stolen from No Mutants Allowed:
//  http://nma-fallout.com/forum/viewtopic.php?t=12372
//
// D = 4096; // Dictionary (a.k.a. sliding window / ring / buffer) size
// All variables (FLAGS, N, O, L) initialized as 0.
//	
//	REMEMBER! When writing to output below and after 2.2. (N > 0), write
//	every byte also in dictionary, advancing dictionary offset by one.
//	Once offset reaches the end of the dictionary, drop it back to the
//	start (normally 0).
//
//	REMEMBER! Clearing dictionary means replacing it's contents with
//	spaces (character #32, hex 0x20), not 0 or other value!
//
//	0. Set dictionary offset to D-18
//	1. If at the end of file, exit (duh).
//	2. Read N (2 bytes == word) from input. The absolute value of N is how
//	   many bytes of data to read (if N=0, exit, duh)
//	2.1. If N < 0, read the absolute value of N bytes from input and write
//	     to output (do not put into dictionary). Go to 1.
//	2.2. If N > 0, clear dictionary, repeat below until N bytes have been
//	     read from input, and then go to 1.
//	2.2.1. Read FLAGS (1 byte) from input. Repeat below 8 times.
//	2.2.1.1. If FLAGS *is not* a modulus of 2 (& 1 != 0), read 1 byte from
//	         input, write to output, then go to 2.2.1.3.
//	2.2.1.2. If FLAGS *is* a modulus of 2 (& 1 == 0)
//	2.2.1.2.1. Read O (1 byte) and L (1 byte) from input.
//	2.2.1.2.2. Take away the High-nibble (first 4 bits) from L and prepend
//	           it to O;
//	2.2.1.2.3. Read L+3 bytes from dictionary at offset O, and write them
//	           to the output.
//	2.2.1.3. Divide FLAGS by 2, rounding down (>> 1).
//	
//	(Note: We read L+2 in 2.2.1.2.3.)

	bool LZSSDecoder::LZSSDecode (uint8_t* in, long nInputLength, uint8_t* out) {
		static const long rb_base = RING_SIZE - 18; 
		// the upper limit on match length

		off_t inpos = 0;
		off_t curbase = m_outindex;
		unsigned int flags = 0;

		while (inpos < nInputLength) {
			int i;
			int j;

			if( m_outindex > m_outlen ) {
				Warn( __FUNCTION__ ) << "Trying to write to index " << m_outindex << " of " << m_outlen;
				return false;
			}

			// Every eight bytes, we have a "reference mask" that
			// gives an information whether data is a reference or
			// a raw byte. A 1 in the map indicates a raw byte,
			// a 0 means a backward reference.
			//
			// The "high" byte of the flags variable is used as a
			// counter to eight, the low byte contains the map.
			flags >>= 1;
			if (!(flags & 0x0100)) {
				unsigned int c = in[inpos++];

				// reset the "high" byte 
				flags = c | 0xff00;
			}

			// "normal" byte (no compression marker)
			if (flags & 1) {
				out[m_outindex] = in[inpos];
				++m_outindex;
				++inpos;
				continue;
			}

			// backward reference
			//
			// the reference is encoded in 2 consecutive bytes.
			// The elements of the reference are i and j, with i
			// being the index into the dictionary and j the length
			// of the entry. Those numbers are encoded into 16
			// bytes like this pattern:
			//  0xabcd => i = 0xcab, j = 0xd
			// j is added to the reference threshold (2).
			// 
			// i+k defines an absolute address *in an imaginary
			// ringbuffer*, meaning that the addresses have to be
			// converted for use with a sliding window.
			if (inpos+2 > nInputLength) {
				//Warn( __FUNCTION__ ) << "Got EOF while expecting reference.";
				break;
			}

			i = in[inpos++];
			j = in[inpos++];

			i |= ((j & 0xf0) << 4);
			j = (j & 0x0f) + THRESHOLD;

			assert( i >= 0 );
			assert( j >= 0 );

			// check for output overflow
			if( m_outindex + j > m_outlen ) {
				Warn( __FUNCTION__ ) << "Trying to write overly long reference of " << j << " bytes at index " << m_outindex << " (at " << m_outindex << " of " << m_outlen << ")";
				return false;
			}

			off_t chunkidx = m_outindex - curbase;

			// virtual ringbuffer base index.
			// Gives the current write position inside a ringbuffer
			// that we aren't using but that the compression
			// scheme implies.
			unsigned long rb_index = (rb_base + chunkidx) % RING_SIZE;

			// addressing base index.
			// Gives the absolute read position in the output
			// stream that needs to be used to read ring buffer
			// address i
			off_t base_index = m_outindex - ((rb_index - i) % RING_SIZE);

			off_t k;
			for (k = 0; k <= j && base_index + k < 0; ++k) {
				out[m_outindex] = '\x20';
				++m_outindex;
			}

			for (; k <= j; ++k) {
				out[m_outindex] = out[base_index + k];
				++m_outindex;
			}
		}

		return true;
	}

} } } } // FIFE::map::loaders::fallout

